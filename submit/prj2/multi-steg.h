#ifndef MULTI_STEG_H_
#define MULTI_STEG_H_

#include "cmd-args.h"
#include "steg.h"


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
void do_multi_steg(const CmdArgs *cmd);


#endif //ifndef MULTI_STEG_H_
