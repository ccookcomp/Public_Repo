#ifndef STEG_SERVER_H_
#define STEG_SERVER_H_

#include <stdbool.h>
#include <sys/types.h>

/** Command-line options used when starting stegd server */
typedef struct {
  bool isQuiet;   /** if true, then do not output worker errors on stderr */
} StegServerOpts;

/** Start a daemon server process running in serverDir as per options
 *  opts.  Return pid of daemon process; < 0 on error.
 */
pid_t steg_serve(const char *serverDir, const StegServerOpts *opts);

#endif //ifndef STEG_SERVER_H_

