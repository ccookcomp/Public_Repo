#ifndef FIFOS_H_
#define FIFOS_H_

#include <stdbool.h>
#include <stdio.h>

#include <sys/types.h>


/************************** Fifos Information **************************/

/** Number of private FIFOs */
enum { N_FIFOS = 2};

/** Information for private FIFO's */
typedef struct {
  //[0] for input, [1] for output; directions always relative to using process
  const char *paths[N_FIFOS]; /** path names for FIFOs */
  FILE *ios[N_FIFOS];         /** FILE streams for FIFOS: [0]: in, [1]: out */
} Fifos;

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
char *make_fifos(pid_t pid, const char *serverDir, Fifos **fifos);


/** Open FIFOs.  To get correct blocking behavior, if isClient is
 *  true, open input FIFO first and then output FIFO; if isClient is
 *  false, open output FIFO first and then input FIFO. After this
 *  call, the ios[] member of fifos are ready for use.  Returns NULL
 *  if everything ok, suitable dynamically allocated error message on
 *  error.
 */
char *open_fifos(Fifos *fifos, bool isClient);

/** Close fifos->ios[] FILE streams for useful ends of FIFOs, as well
 *  as descriptors for non-useful ends of FIFOs.  Returns NULL if
 *  everything ok, suitable dynamically allocated error message on
 *  error.
 */
char *close_fifos(Fifos *fifos);

/** Free up all resources using by fifos.  If FIFOs were created by
 *  calling process, then unlink them. Returns NULL if everything
 *  ok, suitable dynamically allocated error message on error.
 */
char *free_fifos(Fifos *fifos);

#endif //ifndef FIFOS_H_

