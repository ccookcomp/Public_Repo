#include "sized-data.h"

#include "errors.h"

//#define DO_TRACE 1
#include "trace.h"

#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <unistd.h>

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
char *
in_sized_data(FILE *in, SizedData **dataP)
{
  char *err = NULL;
  SizedData *data = NULL;
  size_t size;
  int nRead = fscanf(in, "%zu\n", &size);
  if (nRead != 1) {
    if (nRead != EOF || !feof(in)) {
      aerror(&err, "cannot read size (ret value %d) for sizedBuf() from fd %d:",
             nRead, fileno(in));
      goto CLEANUP;
    }
    data = NULL; //indicate EOF without any error
    goto FINALLY;
  }
  size_t allocSize = sizeof(SizedData) + size;
  data = malloc(allocSize);
  if (!data) {
    aerror(&err, "cannot malloc(%zu) for in_sized_data():", allocSize);
    goto CLEANUP;
  }
  if (fread(data->data, size, 1, in) != 1) {
    aerror(&err, "cannot read content for sizedBuf():");
  }
  data->size = size;
  TRACE("%ld: in_sized_data(): read %zu chars", (long)getpid(), data->size);
  goto FINALLY;
 CLEANUP:
  if (data) { free(data); data = NULL; }
  return err;
 FINALLY:
  *dataP = data;
  return err;
}

/** Serialize bytes from data[size] to FILE stream out.
 *
 *  Returns NULL if everything ok, suitable dynamically allocated
 *  error message on error.  It is the caller's responsibility
 *  to free() any returned error string.
 */
char *
out_sized_data(FILE *out, const char data[], size_t size)
{
  char *err = NULL;
  if (fprintf(out, "%zu\n", size) < 0) {
    aerror(&err, "cannot write sized-data size %zu to fd %d:",
           size, fileno(out));
    return err;
  }
  if (fwrite(data, size, 1, out) != 1) {
    aerror(&err, "cannot write sized-data data of size %zu to fd %d:",
           size, fileno(out));
    return err;
  }
  if (fflush(out) != 0) {
    aerror(&err, "cannot flush sized-data %zu to fd %d:", size, fileno(out));
    return err;
  }
  TRACE("%ld: out_sized_data(): wrote %zu chars", (long)getpid(), size);
  return err;
}

