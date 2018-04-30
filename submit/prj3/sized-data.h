#ifndef SIZED_DATA_H_
#define SIZED_DATA_H_

#include <stdio.h>

/** A flexi-array with size. */
typedef struct {
  size_t size;  //# of data[] bytes.
  char data[];  //actual data.
} SizedData;

/** A SizedData is serialized starting with a line containing the size
 *  (in ASCII) followed by a newline.  The newline is followed by
 *  exactly size binary bytes of data.
 */

/** Set *data to point to dynamically allocated SizedData deserialized
 *  from FILE stream in. It is the caller's responsibility to free()
 *  *data.
 *
 *  If EOF encountered on in, returns no error and sets *data to NULL.
 *
 *  Returns NULL if everything ok, suitable dynamically allocated
 *  error message on error.  It is the caller's responsibility to
 *  free() any returned error string.
 */
char *in_sized_data(FILE *in, SizedData **data);

/** Serialize bytes from data[size] to FILE stream out.
 *
 *  Returns NULL if everything ok, suitable dynamically allocated
 *  error message on error.  It is the caller's responsibility
 *  to free() any returned error string.
 */
char *out_sized_data(FILE *out, const char data[], size_t size);


#endif //ifndef SIZED_DATA_H_

