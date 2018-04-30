#include "common.h"
#include "sized-data.h"
#include "steg-server.h"
#include "file-io.h"
#include "steg3.h"
#include "fifos.h"
 

#include "trace.h"
#include "errors.h"

#include <assert.h>
#include <dlfcn.h>
#include <errno.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define STEG_FN_NAME "new_steg"
#define STEG_STATUS_FN_NAME "steg_status"

//#define DO_TRACE 1
/** Start a daemon server process running in serverDir as per options
 *  opts.  Return pid of daemon process; < 0 on error.
 */
typedef struct {
  Fifos *fifos;                /** information for fifos */
  SizedData *moduleData;       /** path for dynamically loaded module */
  SizedData *boolData;
  SizedData *messageData;
  SizedData *byteData;
  void *module;                /** handle for dynamically loaded module */
  char boo;
  size_t nBytes;
  StegStatus (*newSteg)(Byte[],int,Steg**);                  /** Steg function */
  const char *(*steg_status)(StegStatus);     /** function for translating map error status*/
  const char *bytes;
  const char *msg;
} RequestInfo;

static void die(RequestInfo *req, bool isDomainError, const char *fmt, ...);

static RequestInfo *make_request_info(long clientPid){
  RequestInfo *req = calloc(1, sizeof(RequestInfo));
  if (!req) {
    die(req, false, "%s: cannot calloc(%zu): %s",
        __func__, sizeof(RequestInfo), strerror(errno));
  }
  const char *err = make_fifos((pid_t)clientPid, ".", &req->fifos);
  if (err) die(req, false, "%s: cannot make fifos: %s", __func__, err);
  Fifos *fifos = req->fifos;
  err = open_fifos(fifos, false);
  if (err) die(req, false, err);
  FILE *in = fifos->ios[0];
  err = in_sized_data(in, &req->moduleData);
  if (err) {
    die(req, false, "%s: cannot get module path from client: %s",
        __func__, err);
  }
  const char *moduleName = req->moduleData->data;
  req->module = dlopen(moduleName, RTLD_NOW);
  if (!req->module) {
    die(req, true, "%s: cannot load %s: %s", __func__, moduleName, dlerror());
  }
  req->newSteg = dlsym(req->module, STEG_FN_NAME);
  if (!req->newSteg) {
    die(req, false, "%s: cannot resolve module fn %s: %s",
        __func__, STEG_FN_NAME, dlerror());
  }
  req->steg_status = dlsym(req->module, STEG_STATUS_FN_NAME);
  if (!req->steg_status) {
    die(req, false, "%s: cannot resolve module fn %s: %s",
        __func__, STEG_STATUS_FN_NAME, dlerror);
  }
  TRACE("%ld: loaded module %s and resolved %s and %s",
        (long)getpid(), moduleName, STEG_FN_NAME, STEG_STATUS_FN_NAME);
  err = in_sized_data(fifos->ios[0], &req->boolData);
  //sleep(12);
  if (err) {
    die(req, false, "%s: cannot get bool from client: %s",
        __func__, err);
  }
  req->boo = req->boolData->data[0];
  //fatal("%c\n",req->generalData->data[0]);
  //free(req->generalData);
  //req->boo = '1';
  if(req->boo=='1'){
		//fatal("%d\n",req->boo);
		err = in_sized_data(fifos->ios[0], &req->messageData);
	  if (err) {
		die(req, false, "%s: cannot get message from client: %s",
		    __func__, err);
 	 }
  	req->msg = req->messageData->data;
  	//fatal("%s\n",req->msg);
  	//free(req->messageData);
  }
	//fatal("HELLO");
	err = in_sized_data(fifos->ios[0], &req->byteData);
  if (err) {
    die(req, false, "%s: cannot get image bytes from client: %s",
        __func__, err);
  }
  //fatal("%d\n" , req->generalData->size);
  req->bytes = req->byteData->data;
  req->nBytes= req->byteData->size;
  //free(req->generalData);
  //fatal("HELLO");
  //fatal("%d\n",req->nBytes);
  //TRACE("%ld: read %d ints", (long)getpid(), req->numInts->nInts);
  return req;
}

static char *free_request_info(RequestInfo *req){
  char *err = NULL;
  assert(req != NULL);
  if (req->module && dlclose(req->module) != 0) {
    aerror(&err, "unable to unload %s: %s", req->moduleData->data, dlerror());
  }
  if (req->fifos) {
    char *closeErr = close_fifos(req->fifos);
    if (closeErr) aerror(&err, "%s", closeErr);
    char *freeErr = free_fifos(req->fifos);
    if (freeErr) aerror(&err, "%s", freeErr);
    req->fifos = NULL;
  }
  if (req->newSteg) free((void *)req->newSteg);
  if (req->moduleData) free((void *)req->moduleData);
  if (req->moduleData) free((void *)req->boolData);
  if (req->moduleData) free((void *)req->messageData);
  if (req->moduleData) free((void *)req->byteData);
  free(req);
  return err;
}

static void
die(RequestInfo *req, bool isDomainErr, const char *fmt, ...)
{
  TRACE("%ld: die %s", (long)getpid(), fmt);
  va_list ap;
  if (!isDomainErr || !req) {
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    va_end(ap);
  }
  va_start(ap, fmt);
  size_t n = vsnprintf(NULL, 0, fmt, ap);
  va_end(ap);
  assert(n > 0);
  char msg[n + 1];
  va_start(ap, fmt);
  size_t n1 = vsnprintf(msg, n+1, fmt, ap);
  assert(n1 == n);
  va_end(ap);
  if (req && req->fifos) {
    FILE *out = req->fifos->ios[1];
    if (out) {
      fprintf(out, "%d\n", ERR_STATUS);
      const char *err = out_sized_data(out, msg, n + 1);
      TRACE("%ld: msg = %s", (long)getpid(), msg);
      if (err) error("%s: error in writing to client FIFO: %s", __func__, err);
    }
  }
  if (req) free_request_info(req); //ignore free-err.
  exit(1);
}

static void do_steg_hide(RequestInfo *req){
    int n_bytes = req->nBytes;
	//StegStatus status;
    Byte byte[n_bytes];
	Byte ppm[n_bytes];
    const char *final;
    for(int i=0; i<n_bytes;i++) byte[i]=(Byte)req->bytes[i];
	Steg *s1;
	StegStatus ss;
	ss = req->newSteg(byte, n_bytes, &s1);
	if (ss != STEG_OK) {
      die(req, true, "%s: cannot steg %s: %s",
          __func__, req->msg, req->steg_status(ss));
    }
	ss = s1->fns->hide(s1, req->msg, ppm);
	if (ss != STEG_OK) {
      die(req, true, "%s: cannot steg %d: %s",
          __func__, req->msg, req->steg_status(ss));
    }
	FILE *out = req->fifos->ios[1];
	fprintf(out, "%d\n", OK_STATUS);
	s1->fns->free(s1);
	final=(const char*)ppm;
 	char *err = out_sized_data(req->fifos->ios[1], final, n_bytes);
  	if (err) die(req, false, err);
	}

static void do_steg_unhide(RequestInfo *req){
	int n_bytes = req->nBytes;
	const char *final;
	Byte byte[n_bytes];
    for(int i=0; i<n_bytes;i++) byte[i]=(Byte)req->bytes[i];
	Steg *s1;
	StegStatus ss;
	ss = req->newSteg(byte, n_bytes, &s1);
	if (ss != STEG_OK) {
      die(req, true, "%s: cannot steg %s: %s",
          __func__, req->msg, req->steg_status(ss));
    }
	size_t ms;
	s1->fns->max_msg_size(s1,&ms);
	if (ss != STEG_OK) {
      die(req, true, "%s: cannot steg %s: %s",
          __func__, req->msg, req->steg_status(ss));
    }
	char msg[ms];
	ss = s1->fns->unhide(s1,msg);
	if (ss != STEG_OK) {
      die(req, true, "%s: cannot steg %s: %s",
          __func__, req->msg, req->steg_status(ss));
    }
	fprintf(req->fifos->ios[1], "%d\n", OK_STATUS);
	final = (const char *)msg;
 	char *err = out_sized_data(req->fifos->ios[1], final, strlen(final)+1);
	s1->fns->free(s1);
  	if (err) die(req, false, err);

	}

static void do_steg(RequestInfo *req){
	if(req -> boo=='1'){ //hide
		do_steg_hide(req);
	}
	else{ // unhide
		//fatal("HELLO");
		do_steg_unhide(req);
	}
}

static void do_worker(long clientPid){
  TRACE("%ld: worker starting for client %ld", (long)getpid(), clientPid);
  RequestInfo *req = make_request_info(clientPid);
  do_steg(req);
  free_request_info(req);
}

static void make_worker(long clientPid, FILE *requests){
  //sleep(10);
  pid_t childPid = fork();
  if (childPid < 0) {
    error("daemon child fork error:");
  }
  else if (childPid == 0) { //immediate daemon child
    if (fclose(requests) != 0) {
      fatal("daemon child %ld cannot close requests FIFO:", (long)getpid());
    }
    pid_t workerPid = fork();
    if (workerPid < 0) fatal("daemon child fork error:");
    else if (workerPid == 0) { //daemon grandchild: worker process
      do_worker(clientPid);
      exit(0);
    }
    else { //daemon child
      exit(0);
    }
  }
	else { //daemon process: wait for immediate child
		int status;
		if (waitpid(childPid, &status, 0) < 0 || status != 0) {
		  error("daemon wait error:");
		}
	}
}
static void do_daemon(FILE *requests){
  while (1) { //infinite service loop
    TRACE("%ld: daemon processing loop", (long)getpid());
    long pid;
    if (fscanf(requests, "%ld", &pid) != 1) {
      error("cannot read initial requests");
    }
	//pid=getpid();
    TRACE("%ld: daemon processing client %ld", (long)getpid(), (long)pid);
    make_worker(pid, requests);
  }
}

static void make_blocking(const char *name, int fd){
  int flags = fcntl(fd, F_GETFL, 0);
  if (flags < 0) fatal("cannot get flags for  %s(%d):", name, fd);
  if (fcntl(fd, F_SETFL, flags & ~O_NONBLOCK) < 0) {
    fatal("cannot set %s(%d) unblocking:", name, fd);
  }
}

static FILE *open_requests(const char *fifoPath){
  //opening fifo O_RDWR not defined by POSIX for FIFO; hence portably
  //get effect of O_RDWR by first non-blocking open O_RDONLY and then
  //open O_WRONLY.
  int fd = open(fifoPath, O_RDONLY|O_NONBLOCK);
  if (fd < 0) fatal("cannot read %s:", fifoPath);
  if (open(fifoPath, O_WRONLY) < 0) { //no block since already reading
    fatal("cannot write %s:", fifoPath);
   }
  make_blocking(fifoPath, fd);
  FILE *requests = fdopen(fd, "r");
  if (requests == NULL) fatal("cannot fdopen(%d) for %s:", fd, fifoPath);
  return requests;
}

static pid_t make_daemon(const char *reqFifoPath){
  pid_t pid = fork();
  if (pid < 0) {
    error("cannot fork daemon:");
    return -1;
  }
  else if (pid == 0) { //daemon process
    FILE *requests = open_requests(reqFifoPath);
    if (setsid() < 0) fatal("cannot create new session:");
    do_daemon(requests); //should not return
    fatal("unexpected daemon exit:");
  }
  //parent
  return pid;
}

pid_t steg_serve(const char *serverDir, const StegServerOpts *opts){
  if (chdir(serverDir) != 0) {
    error("cannot cd to %s:", serverDir);
    return -1;
  }
  if (mkfifo(REQUESTS_FIFO, 0666) < 0 && errno != EEXIST) {
    error("cannot create FIFO %s/%s:", serverDir, REQUESTS_FIFO);
    return -1;
  }
  pid_t pid = make_daemon(REQUESTS_FIFO);
  return pid;
}

