#ifndef COMMON_H_
#define COMMON_H_

#include <stdbool.h>
#include <stdio.h>

#include <sys/types.h>

/** Basic request */
typedef enum { HIDE_REQ, UNHIDE_REQ } StegCmd;

/** Return status of a request. */
enum { OK_STATUS, ERR_STATUS };

/************************** Fifos Information **************************/

/** Name of well-known requests FIFO in server-dir used by both clients
 *  and server.
 */
#define REQUESTS_FIFO "REQUESTS"

//TODO: add declarations common between client and server here

#endif //ifndef COMMON_H_

