#include "common.h"
#include "steg-client.h"
#include "sized-data.h"
#include "fifos.h"
#include "steg3.h"
 

#include "errors.h"
#include "memalloc.h"
#include "trace.h"

#include <assert.h>
#include <string.h>

#include <sys/types.h>
#include <unistd.h>

typedef struct {
  const char *serverDir;
  const char *stegMod;
  Fifos *fifos;
} Client;

static char *new_client(const char *serverDir, const char *stegMod, Client **clientP){
  char *err = NULL;
  Client *client = malloc(sizeof(Client));
  if (client == NULL) {
    aerror(&err, "cannot alloc memory for client:");
  }
  else {
    *clientP = client;
    client->serverDir = serverDir;
    client->stegMod = stegMod;
    if ((err = make_fifos(-1, serverDir, &client->fifos)) != NULL) free(client);
  }
  return err;
}

static char *free_client(const Client *client){
  char *err = NULL;
  char *closeErr = close_fifos(client->fifos);
  if (closeErr) aerror(&err, "%s", closeErr);
  char *freeErr = free_fifos(client->fifos);
  if (freeErr) aerror(&err, "%s", freeErr);
  free((void *)client);
  return err;
}


static char *send_init_request(const Client *client){
  char *err = NULL;
  FILE *req = NULL;
  int reqPathLen = snprintf(NULL, 0, "%s/%s", client->serverDir, REQUESTS_FIFO);
  assert(reqPathLen > 0);
  char reqPath[reqPathLen + 1];
  int n = snprintf(reqPath, sizeof(reqPath), "%s/%s",
                   client->serverDir, REQUESTS_FIFO);
  assert(n == reqPathLen);
  req = fopen(reqPath, "w");
  if (!req) {
    aerror(&err, "cannot open %s:", reqPath);
    goto FINALLY;
  }
  if (fprintf(req, "%ld\n", (long)getpid()) < 0) {
    aerror(&err, "cannot write PID to %s:", reqPath);
    goto FINALLY;
  }
 FINALLY:
  if (fclose(req) != 0) aerror(&err, "cannot close %s:", reqPath);
  return err;
}

static char *send_request(const Client *client, size_t nBytes, const char bytes[nBytes], const char *msg){
  char *err = NULL;
  FILE *out = client->fifos->ios[1];
  const char *outPath = client->fifos->paths[1];
  err = out_sized_data(out, client->stegMod, strlen(client->stegMod) + 1);
  if (err != NULL) {
    aerror(&err, "cannot write module name %s to FIFO %s:",
           client->stegMod, outPath);
    goto CLEANUP;
  }
  if(msg != NULL){
    char msg1[strlen(msg)+1];
  for (unsigned int i=1; i<strlen(msg);i++) msg1[i-1] = msg[i];
  const char *newMess = msg1;
	const char boo[]={'1','\0'};
	  if ((err = out_sized_data(client->fifos->ios[1], boo , strlen(boo)+1)) != NULL) {
		aerror(&err, "cannot write %zu ints to FIFO %s:", nBytes, outPath);
		goto CLEANUP;
	  }
		//printf("%s\n",msg);
		//msg[strlen(msg)]='\0';
	  if ((err = out_sized_data(client->fifos->ios[1], newMess, strlen(newMess)+1)) != NULL) {
		aerror(&err, "cannot write %zu ints to FIFO %s:", nBytes, outPath);
		goto CLEANUP;
	  }

		//printf("%s\n",msg[12]);
	}else{
		const char boo[]={'0','\0'};
			if ((err = out_sized_data(client->fifos->ios[1], boo, strlen(boo)+1)) != NULL) {
		aerror(&err, "cannot write %zu ints to FIFO %s:", nBytes, outPath);
		goto CLEANUP;
	  }
	}
	//printf("%s\n",bytes);
	if ((err = out_sized_data(client->fifos->ios[1], bytes, nBytes)) != NULL) {
    aerror(&err, "cannot write %zu ints to FIFO %s:", nBytes, outPath);
    goto CLEANUP;
  }
	//printf("%s\n", bytes);
	//printf("%d\n", nBytes);
  goto FINALLY;
 CLEANUP:
 FINALLY:
  return err;
}

static char *do_client(const char *serverDir, const char *stegMod,
          size_t nBytes, const char bytes[nBytes], Client **clientP,
			const char *msg){
  Client *client = NULL; 
  char *err = new_client(serverDir, stegMod, &client);
  if (err) {
    aerror(&err, "cannot create client in dir %s with module %s",
           serverDir, stegMod);
    goto CLEANUP;
  }
  if ((err = send_init_request(client)) != NULL) {
    aerror(&err, "cannot send init request for client in dir %s with module %s",
          serverDir, stegMod);
    goto CLEANUP;
  }
  if ((err = open_fifos(client->fifos, true)) != NULL) {
    aerror(&err, "cannot open fifos for client in dir %s with module %s",
          serverDir, stegMod);
    goto CLEANUP;
  }
  if ((err = send_request(client, nBytes, bytes, msg)) != NULL) {
    aerror(&err, "error sending request to server in dir %s with module %s",
           serverDir, stegMod);
    goto CLEANUP;
  }
  *clientP = client;
  goto FINALLY;
 CLEANUP:
  if (client) {
    char *freeErr = free_client(client);
    if (freeErr) aerror(&err, "%s", freeErr);
  }
 FINALLY:
  return err;
}

static char *
get_server_response(const Client *client, SizedData **resultP)
{
  //sleep(12);
  char *err = NULL;
  FILE *in = client->fifos->ios[0];
  //NumInts *result = NULL;
  SizedData *result=NULL;
  SizedData *serverErr = NULL;
  int status;
  //int res;
  //printf("%d\n",feof(in));
  //printf("%c\n",fgetc(in));
  //if(feof(in)==EOF) printf("HELLO");
  //exit(1);
  if (fscanf(in, "%d\n", &status) != 1) {
	//printf("%d",res);
    aerror(&err, "cannot read status from server");
    goto CLEANUP;
  }
  bool isOk = (status == STEG_OK);
  err = isOk ?  in_sized_data(in, &result) : in_sized_data(in, &serverErr);
  //printf("%d\n",result->size);
  if (err) {
    aerror(&err, "%s: cannot read result from server", __func__);
    goto CLEANUP;
  }
  if (isOk) {
    *resultP = result;
  }
  else {
    if (!(err = malloc(serverErr->size))) {
      aerror(&err, "%s: cannot malloc(%zu) for server error",
             __func__, serverErr->size);
      goto CLEANUP;
    }
    memcpy(err, serverErr->data, serverErr->size);
  }
  goto FINALLY;
 CLEANUP:
  if (result) free(result);
 FINALLY:
  if (serverErr) free(serverErr);
  return err;
}

/** Use steganography module stegMod in steganography server running
 *  in directory serverDir to hide NUL-terminated msg using PPM image
 *  specified by bytes[nBytes].
 *
 *  If everything ok, set *result to point to a dynamically allocated
 *  SizedData containing the bytes of a PPM image containing the
 *  hidden message and return NULL.  It is the caller's responsibility
 *  to free() *result.
 *
 *  If not ok, return a dynamically allocated string containing a
 *  suitable error message including any newlines. If the error is
 *  detected by the stegMod module, then return the exact string
 *  returned by its steg_status() function. It is the caller's
 *  responsibility to free() this error string.
 *
 *  It is assumed that module stegMod is available on the
 *  LD_LIBRARY_PATH for the steganography server running in directory
 *  serverDir.
 */
char *
stegc_hide(const char *serverDir, const char *stegMod,
           size_t nBytes, const char bytes[nBytes],
           const char *msg, SizedData **result){
	//printf("%d\n", nBytes);
  char *err = NULL;
  Client *client;
  err = do_client(serverDir, stegMod,  nBytes, bytes, &client, msg);
  if (err != NULL) return err;
  if (!err) err = get_server_response(client, result);
  char *freeErr = free_client(client);
  return err ? err : freeErr;
  //return NULL; //replace placeholder return
}

/** Use steganography module stegMod in steganography server running
 *  in directory serverDir to unhide a message from PPM image
 *  specified by bytes[nBytes].
 *
 *  If everything ok, set *result to point to a dynamically allocated
 *  SizedData containing the characters of the recovered image and
 *  return NULL.  It is the caller's responsibility to free() *result.
 *
 *  If not ok, return a dynamically allocated string containing a
 *  suitable error message including any newlines. If the error is
 *  detected by the stegMod module, then return the exact string
 *  returned by its steg_status() function. It is the caller's
 *  responsibility to free() this error string.
 *
 *  It is assumed that module stegMod is available on the
 *  LD_LIBRARY_PATH for the steganography server running in directory
 *  serverDir.
 *
 */
char *stegc_unhide(const char *serverDir, const char *stegMod,
             size_t nBytes, const char bytes[nBytes],
             SizedData **result){
  char *err = NULL;
  Client *client;
  err = do_client(serverDir, stegMod,  nBytes, bytes, &client, NULL);
  if (err != NULL) return err;
  if (!err) err = get_server_response(client, result);
  char *freeErr = free_client(client);
  return err ? err : freeErr;
  //return NULL; //replace placeholder return
}


