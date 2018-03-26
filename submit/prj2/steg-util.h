#ifndef STEG_UTIL_H_
#define STEG_UTIL_H_

#include "steg.h"

#include <stdio.h>

/** Error handling for this module: If any routine encounters an
 *  error, it terminates the current process with an error message.
 */

/***************************** Sized Data ******************************/

/** A flexi-array with size; used for both messages and images sent
 *  over pipes.
 */
typedef struct {
  size_t size;  //# of data[] bytes not including convenience NUL character
  char data[];  //will always be NUL-terminated, so can be used for strings
} SizedData;

/** Return dynamically allocated SizedData read from in. It is the
 *  caller's responsibility to free() the return value. Return NULL
 *  on EOF.
 */
const SizedData *in_sized_data(FILE *in);

/** Output bytes from data[size] to out */
void out_sized_data(FILE *out, const char data[], size_t size);

/************** Message Serialization / Deserialization ****************/

/** A message is serialized to a stream in the following format:
 *
 *     msgLen:      length of message as an ASCII integer.
 *     whitespace:  a single whitespace character
 *     msg:         msgLen bytes constituting the message characters.
 */

/** Deserialize a message from the FILE stream f returning it in a
 *  dynamically allocated SizedData object.  Returns NULL on EOF. Note
 *  that the data[] in the returned SizedData object is
 *  NUL-terminated.  It is the caller's responsibility to free() the
 *  returned value.
 */
const SizedData *deserialize_msg_stream(FILE *f);

/** Serialize message msg having length msgLen onto the FILE stream
 *  f.
 */
void serialize_msg_stream(FILE *f, const char *msg, size_t msgLen);

/**************** Steg Serialization / Deserialization *****************/

/** A steg object is serialized to a stream with the following:
 *
 *     idLen:       length of its id as an ASCII integer.
 *     whitespace:  a single whitespace character
 *     id:          idLen bytes constituting the id characters.
 *     nBytes:      total # of bytes of its underlying PPM image.
 *     whitespace:  a single whitespace character
 *     bytes:       nBytes bytes constituting the bytes of underlying
 *                  PPM image.
 */

/** Deserialize a new Steg object from the FILE stream f.  Return NULL
 *  on EOF.  It is the caller's responsibility to call
 *  steg->fns->free() on the return'd steg object.
 */
const Steg *deserialize_steg_stream(FILE *f);

/** Serialize Steg object steg onto the FILE stream f. */
void serialize_steg_stream(FILE *f, const Steg *steg);

/*********************** Miscellaneous Routines ************************/

/** Return id for steg. */
const char *steg_id(const Steg *steg);

/** Return # of bytes for PPM image underlying steg. */
size_t steg_nbytes(const Steg *steg);

/** Return max message size for steg. */
size_t max_steg_msg_size(const Steg *steg);

/** Create a new Steg object from the PPM image stored in path with
 *  its id set to the basename of path (without the extension).  It is
 *  the caller's responsibility to call steg->fns->free() on the
 *  returned steg object.
 */
const Steg *new_steg_from_file(const char *path);


#endif //ifndef STEG_UTIL_H_

