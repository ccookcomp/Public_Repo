//project includes
#include "cmd-args.h"
#include "multi-steg.h"
#include "steg-util.h"

//uncomment following line to activate TRACE(...) calls in code
//#define DO_TRACE 1

//course libraries includes
#include "errors.h"
#include "file-io.h"
#include "memalloc.h"
#include "steg.h"
#include "trace.h"

//C library includes
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//os includes
#include <sys/types.h>
#include <unistd.h>


typedef struct {
  int index;      //worker index
  int messLen;
  int inPipe[2];  //from parent to child
  int outPipe[2]; //from child to parent   
  FILE *in;		  //relative to owning process
  FILE *out;      //relative to owning process
  //FILE *img;
  const char *file;    
  char *text;	  //Text to be written or read in
} WorkerInfo;

static void setup_parent0(WorkerInfo *wP)
{
  if (close(wP->inPipe[0]) != 0) {
    fatal("parent close inPipe[0] %d:", wP->index);
  }
  if (close(wP->outPipe[1]) != 0) {
    fatal("parent close outPipe[1] %d:", wP->index);
  }
  
}

static void setup_parent1(WorkerInfo infos[], int n){
  for (int i = 0; i < n; i++) {
    WorkerInfo *wP = &infos[i];
    if (!(wP->in = fdopen(wP->outPipe[0], "r"))) {
      fatal("parent fdopen(in) %d:", wP->index);
    }
    if (!(wP->out = fdopen(wP->inPipe[1], "w"))) {
      fatal("parent fdopen(out) %d:", wP->index);
    }
  }
}


static void cleanup_parent(const WorkerInfo infos[], int n){
  for (int i = 0; i < n; i++) {
    fclose(infos[i].out); //not checking error
  }
  for (int i = 0; i < n; i++) {
    int dummy;
    int ret = fscanf(infos[i].in, "%d", &dummy);
    if (ret != EOF || !feof(infos[i].in)) {
      fatal("parent expected EOF from child %d", i);
    }
    fclose(infos[i].in); //not checking error
  }
  //fprintf(out, "parent %ld done\n", (long)getpid());
}


static void setup_child(int index, WorkerInfo infos[]){
  for (int i = 0; i <= index; i++) {
    WorkerInfo *wP = &infos[i];
    if (close(wP->inPipe[1]) != 0) {
      fatal("child close inPipe[1] %d:", wP->index);
    }
    if (close(wP->outPipe[0]) != 0) {
      fatal("child close outPipe[0] %d:", wP->index);
    }
  }
  WorkerInfo *wP = &infos[index];
  if (!(wP->in = fdopen(wP->inPipe[0], "r"))) {
    fatal("child fdopen(in) %d:", wP->index);
  }
  if (!(wP->out = fdopen(wP->outPipe[1], "w"))) {
    fatal("child fdopen(out) %d:", wP->index);
  }
}


static void cleanup_child(const WorkerInfo *wP){
  if (fclose(wP->in) != 0) {
    fatal("child %d close(in):", wP->index);
  }
  if (fclose(wP->out) != 0) {
    fatal("child %d close(out):", wP->index);
  }
}

static void do_child_unhide(const WorkerInfo *wP){ // Needs to be idle waiting
	char c;
	while(read(wP->inPipe[0], &c, 1) != 0){
		//printf("Hello!\n");
		size_t imgSize;
		const Steg *s1=deserialize_steg_stream(wP->in);
		imgSize=max_steg_msg_size(s1);
		char message[imgSize];
		s1->fns->unhide(s1,message);
		write(wP->outPipe[1],"Y",1);
		serialize_msg_stream(wP->out,message, sizeof(message));
		free((void*)s1);
	}
}

static void do_parent_unhide(const CmdArgs *cmd){
	FILE *out = fopen(cmd->msgPath,"w+"); // Add error check
	int n=cmd->nWorkers;
	WorkerInfo infos[n];
	for (int i = 0; i < n; i++) {
		WorkerInfo *wP = &infos[i];
		wP->index = i;
		if (pipe(wP->inPipe) < 0) fatal("inPipe %d:", i);
		if (pipe(wP->outPipe) < 0) fatal("outPipe %d:", i);
		wP->in = wP->out = 0;
		pid_t pid = fork();
		if (pid < 0) {
		  fatal("fork %d:", i);
		}
		else if (pid == 0) { //child
		  setup_child(i, infos);
		  do_child_unhide(wP);
		  cleanup_child(wP);
		  exit(0); //terminate after doChild() returns
		}
		else { //parent
		  setup_parent0(wP);
		}
	  }
  setup_parent1(infos, n);
	char c;
  for (int i = 0; i < cmd->nImgPaths; i++) {
		write(infos[i%n].inPipe[1],"Y",1);
		const Steg *sF=new_steg_from_file(cmd->imgPaths[i]);
		serialize_steg_stream(infos[i%n].out, sF);
		read(infos[i%n].outPipe[0],&c,1);
		const SizedData *sd=deserialize_msg_stream(infos[i%n].in);
		fprintf(out,"%s",sd->data);
		sF->fns->free((Steg*)sF);
		free((void*)sd);
	}
	fclose(out);
    cleanup_parent(infos, n);
}

static void do_child_hide(const WorkerInfo *wP){
	char c;
	while(read(wP->inPipe[0], &c, 1) != 0){
		const SizedData *sd=deserialize_msg_stream(wP->in);
		const Steg *s1=deserialize_steg_stream(wP->in);
		Byte ppm[steg_nbytes(s1)];
		s1->fns->hide(s1,sd->data,ppm);
		write(wP->outPipe[1],"Y",1);
		out_sized_data(wP->out, (char*)ppm, sizeof(ppm));
		s1->fns->free((Steg*)s1);
		free((void*)sd);
	}
}

static void do_parent_hide(const CmdArgs *cmd){
	int n=cmd->nWorkers;
	WorkerInfo infos[n];
	for (int i = 0; i < n; i++) {
		WorkerInfo *wP = &infos[i];
		wP->index = i;
		if (pipe(wP->inPipe) < 0) fatal("inPipe %d:", i);
		if (pipe(wP->outPipe) < 0) fatal("outPipe %d:", i);
		wP->in = wP->out = 0;
		pid_t pid = fork();
		if (pid < 0) {
		  fatal("fork %d:", i);
		}
		else if (pid == 0) { //child
		  setup_child(i, infos);
		  do_child_hide(wP);
		  cleanup_child(wP);
		  exit(0); //terminate after doChild() returns
		}
		else { //parent
		  setup_parent0(wP);
		}
	  }
 	setup_parent1(infos, n);
 	int numFiles=cmd->nImgPaths;
  	size_t characters;
	size_t count;
	const char *in=read_file(cmd->msgPath, &count);
	char *nav=(char*)in;
	const char* inFree = in;
	int numImg=0;
	int b=1;
	int i=0;
	size_t msgSize;
	char c1;
  while(b){
		const Steg *sF=new_steg_from_file(cmd->imgPaths[i%numFiles]);
		characters=max_steg_msg_size(sF);
		char c[characters];
		if((int)count-(int)characters>=0){
			memcpy(c,nav,sizeof(c)-1);
			c[characters-1]='\0';
			nav+=(characters-1);
			count-=(characters-1);
			msgSize=characters;
		}else{
			memcpy(c,nav,sizeof(c)-1);
			b=0;
			c[count]='\0';
			msgSize=count+1;
		}
		//printf("%s",c);
		write(infos[i%n].inPipe[1],"Y",1);
		serialize_msg_stream(infos[i%n].out, c, msgSize);
		serialize_steg_stream(infos[i%n].out, sF);
		read(infos[i%n].outPipe[0],&c1,1);
		const SizedData *sd2=in_sized_data(infos[i%n].in);
		char buf[100];
		snprintf(buf, 100, "%s/%06d.ppm", cmd->destDir,numImg++);
		write_file(buf, sd2->data, sd2->size);
		sF->fns->free((Steg*)sF);
		free((void*)sd2);
		i++;
	}
	free((void*)inFree);
    cleanup_parent(infos, n);
}


/** Perform steganography operation specified by cmd.  Calling
 *  process must fork cmd->nWorkers child worker processes.
 *  Only the parent process may read or write files.
 *  The actual hide/unhide operations must be performed by
 *  the worker processes.
 *
 *  If an error is detected, the program can be terminated after
 *  printing an error message on stderr.  In the interest of
 *  simplicity, it is not necessary to do a clean termination; i.e. if
 *  an error is detected, the program may hang (after printing an
 *  error message), leave extant processes or not clean up allocated
 *  memory.
 */
void
do_multi_steg(const CmdArgs *cmd)
{
	if(cmd->cmd == HIDE_CMD){do_parent_hide(cmd);}
	if(cmd->cmd == UNHIDE_CMD){do_parent_unhide(cmd);}
	//FILE *out;
	
}
