#ifndef STEGC_H_
#define STEGC_H_

#include "sized-data.h"

#include <stdbool.h>
#include <stddef.h>

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
 *  returned by its steg_status() function.  It is the caller's
 *  responsibility to free() this error string.
 *
 *  It is assumed that module stegMod is available on the
 *  LD_LIBRARY_PATH for the steganography server running in directory
 *  serverDir.
 */
char *stegc_hide(const char *serverDir, const char *stegMod,
                 size_t nBytes, const char bytes[nBytes],
                 const char *msg, SizedData **result);

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
                   SizedData **result);


#endif //ifndef STEGC_H_

