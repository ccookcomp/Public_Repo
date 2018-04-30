#include "common.h"
#include "fifos.h"

//#define DO_TRACE 1
#include "errors.h"
#include "trace.h"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


#define FIFO_PATH_FMT "%s/%ld.%s"

typedef struct {
  Fifos;
  bool didCreateFifos;    /** will be true if FIFOs were created */
  char mem[];             /** memory for fifo names */
} FifosData;

static char *
create_fifos(const Fifos *fifos)
{
  char *err = NULL;
  for (int i = 0; i < N_FIFOS; i++) {
    int status = mkfifo(fifos->paths[i], 0666);
    if (status != 0) {
      aerror(&err, "cannot create fifo %s:", fifos->paths[i]);
      for (int j = 0; j < i; j++) unlink(fifos->paths[j]);
      break;
    }
  }
  return err;
}

/** Return dynamically allocated Fifos in *fifos. Assumes used by
 *  client if pid < 0, else assume used by server.  Does not open FIFOs.
 *
 *  When used by client, fifos paths[] set to serverDir/getpid().in,
 *  serverDir/getpid().out (in that order).
 *
 *  When used by server, fifos paths[] set to serverDir/pid.out,
 *  serverDir/pid.in (in that order).  Note that this is the opposite
 *  order from when used by the client.
 *
 *  Returns NULL if everything ok, suitable dynamically allocated
 *  error message on error.
 */
char *
make_fifos(pid_t pid, const char *serverDir, Fifos **fifos)
{
  char *err = NULL;
  int doCreate = pid < 0;
  long lpid = (doCreate) ? getpid() : pid;
  if (!serverDir) serverDir = ".";
  const char *exts[] = { "in", "out" };
  assert(sizeof(exts)/sizeof(exts[0]) == N_FIFOS);
  size_t pathSizes[N_FIFOS];
  size_t totalPathSizes = 0;
  for (int i = 0; i < N_FIFOS; i++) {
    int n = snprintf(NULL, 0, FIFO_PATH_FMT, serverDir, lpid, exts[i]);
    assert(n > 0);
    totalPathSizes += pathSizes[i] = n + 1;
  }
  FifosData *fifosP = calloc(1, sizeof(FifosData) + totalPathSizes);
  if (!fifosP) {
    aerror(&err, "cannot allocate memory for fifos:");
  }
  else {
    size_t size = 0;
    for (int i = 0; i < N_FIFOS; i++) {
      char *path = &fifosP->mem[size];
      size += pathSizes[i];
      int n =
        snprintf(path, pathSizes[i], FIFO_PATH_FMT, serverDir, lpid, exts[i]);
      assert(n == (int)pathSizes[i] - 1);
      fifosP->paths[doCreate ? i : 1 - i] = path;
    }
    assert(size == totalPathSizes);
    if (doCreate && (err = create_fifos((const Fifos *)fifosP)) != NULL) {
      free(fifosP);
    }
    if (!err) {
      fifosP->didCreateFifos = doCreate;
      *fifos = (Fifos *)fifosP;
    }
  }
  return err;
}

/** Open FIFOs.  To get correct blocking behavior, if isClient is
 *  true, open input FIFO first and then output FIFO; if isClient is
 *  false, open output FIFO first and then input FIFO. After this
 *  call, the ios[] member of fifos are ready for use.  Returns NULL
 *  if everything ok, suitable dynamically allocated error message on
 *  error.
 */
char *
open_fifos(Fifos *fifos, bool isClient)
{
  char *err = NULL;
  if (isClient) {
    for (int i = 0; i < N_FIFOS; i++) {
      const char *mode = (i == 0) ? "r" : "w";
      fifos->ios[i] = fopen(fifos->paths[i], mode);
      if (!fifos->ios[i]) {
        aerror(&err, "cannot fopen(%s, %s):", fifos->paths[i], mode);
        goto CLEANUP;
      }
    }
  }
  else {
    for (int j = N_FIFOS; j > 0; j--) {
      int i = (j - 1);
      const char *mode = (i == 0) ? "r" : "w";
      fifos->ios[i] = fopen(fifos->paths[i], mode);
      if (!fifos->ios[i]) {
        aerror(&err, "cannot fopen(%s, %s):", fifos->paths[i], mode);
        goto CLEANUP;
      }
    }
  }
  goto FINALIZE;
 CLEANUP:
  for (int i = 0; i < N_FIFOS; i++) {
    if (fifos->ios[i]) fclose(fifos->ios[i]);
  }
 FINALIZE:
  return err;
}

/** Close fifos->ios[] FILE streams for useful ends of FIFOs, as well
 *  as descriptors for non-useful ends of FIFOs.  Returns NULL if
 *  everything ok, suitable dynamically allocated error message on
 *  error.
 */
char *
close_fifos(Fifos *fifos)
{
  char *err = NULL;
  FifosData *fifosP = (FifosData *)fifos;
  for (int i = 0; i < N_FIFOS; i++) {
    if (fifosP->ios[i] != NULL && fclose(fifosP->ios[i]) != 0) {
      aerror(&err, "cannot fclose useful end of FIFO %s:", fifosP->paths[i]);
    }
    fifosP->ios[i] = NULL;
  }
  return err;
}

/** Free up all resources using by fifos.  If FIFOs were created by
 *  calling process, then unlink them. Returns NULL if everything
 *  ok, suitable dynamically allocated error message on error.
 */
char *
free_fifos(Fifos *fifos)
{
  char *err = NULL;
  FifosData *fifosP = (FifosData *)fifos;
  if (fifosP->didCreateFifos) {
    for (int i = 0; i < N_FIFOS; i++) {
      int status = unlink(fifosP->paths[i]);
      if (status != 0) {
        aerror(&err, "cannot unlink(%s):", fifosP->paths[i]);
      }
    }
  }
  free(fifosP);
  return err;
}

