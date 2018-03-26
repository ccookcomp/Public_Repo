//project includes
#include "steg-util.h"

//uncomment following line to activate TRACE(...) calls in code
//#define DO_TRACE 1

//course library includes
#include "errors.h"
#include "file-io.h"
#include "memalloc.h"
#include "steg.h"
#include "trace.h"

//C library includes
#include <stdio.h>
#include <string.h>

//OS includes
#include <sys/types.h>
#include <unistd.h>


/** If any routine encounters an error, it terminates the current
 *  process with an error message.
 */

/***************************** Sized Data ******************************/

/** Return dynamically allocated SizedData read from in. It is the
 *  caller's responsibility to free() the return value. Return NULL
 *  on EOF.
 */
const SizedData *
in_sized_data(FILE *in)
{
  size_t size;
  char c;
  int nRead = fscanf(in, "%zu%c", &size, &c);
  if (nRead != 2) {
    if (nRead != EOF || !feof(in)) {
      fatal("cannot read size for sizedBuf() from fd %d:", fileno(in));
    }
    return NULL;
  }
  SizedData *data = mallocChk(sizeof(SizedData) + size + 1);
  data->size = size;
  if (fread(&data->data, size, 1, in) != 1) {
    fatal("cannot read content for sizedBuf():");
  }
  data->data[size] = '\0';
  TRACE("<-in_sized_data() %ld: read %ld chars", (long)getpid(), data->size);
  return data;
}

/** Output bytes from data[size] to out */
void
out_sized_data(FILE *out, const char data[], size_t size)
{
  fprintf(out, "%zu\n", size);
  if (fwrite(data, size, 1, out) != 1) {
    fatal("cannot write %zu sized data", size);
  }
  if (fflush(out) != 0) fatal("cannot flush out data[%lu]", size);
  TRACE("<-out_sized_data() %ld: wrote %lu chars", (long)getpid(), size);
}


/************** Message Serialization / Deserialization ****************/

/** A message is serialized to a stream with the following:
 *
 *     msgLen:      length of message as an ASCII integer.
 *     whitespace:  a single whitespace character
 *     msg:         msgLen bytes constituting the id characters.
 */

/** Deserialize a message from the FILE stream f returning it in a
 *  dynamically allocated SizedData object.  Returns NULL on EOF. Note
 *  that the data[] in the returned SizedData object is
 *  NUL-terminated.  It is the caller's responsibility to free() the
 *  returned value.
 */
const SizedData *
deserialize_msg_stream(FILE *f)
{
  return in_sized_data(f);
}

/** Serialize message msg having length msgLen onto the FILE stream
 *  f.
 */
void
serialize_msg_stream(FILE *f, const char *msg, size_t msgLen)
{
  out_sized_data(f, msg, msgLen);
}

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
const Steg *
deserialize_steg_stream(FILE *f)
{
  const SizedData *idData = in_sized_data(f);
  if (idData == NULL) return NULL;
  const SizedData *bytesData = in_sized_data(f);
  if (bytesData == NULL) fatal("no bytes for steg %s", idData->data);
  Steg *steg;
  StegStatus status = new_steg(idData->data, (const Byte *)bytesData->data,
                               bytesData->size, &steg);
  if (status != STEG_OK) {
    fatal("cannot deserialize steg %s: %s", idData->data, steg_status(status));
  }
  free((void *)idData);
  free((void *)bytesData);
  TRACE("<-deserialized_steg_stream() %ld: steg_id=%s",
        (long)getpid(), steg_id(steg));
  return steg;
}

/** Serialize Steg object steg onto the FILE stream f. */
void
serialize_steg_stream(FILE *f, const Steg *steg)
{
  const char *id;
  StegStatus status = steg->fns->id(steg, &id);
  if (status != STEG_OK) {
    fatal("cannot get id for steg: %s", steg_status(status));
  }
  const Byte *bytes;
  size_t nBytes;
  status = steg->fns->bytes(steg, &bytes, &nBytes);
  if (status != STEG_OK) {
    fatal("cannot get bytes for steg %s: %s", id, steg_status(status));
  }
  out_sized_data(f, id, strlen(id));
  out_sized_data(f, (const char *)bytes, nBytes);
  TRACE("<-serialize_steg_stream() %ld: steg_id=%s",
        (long)getpid(), steg_id(steg));
}

/*********************** Miscellaneous Routines ************************/

/** Return id for steg. */
const char *
steg_id(const Steg *steg)
{
  const char *id;
  StegStatus status = steg->fns->id(steg, &id);
  if (status != STEG_OK) fatal("cannot get steg id");
  return id;
}

/** Return # of bytes for PPM image underlying steg. */
size_t
steg_nbytes(const Steg *steg)
{
  size_t nBytes;
  StegStatus status = steg->fns->bytes(steg, NULL, &nBytes);
  if (status != STEG_OK) fatal("cannot get steg nbytes for %s", steg_id(steg));
  return nBytes;
}

/** Return max message size for steg. */
size_t
max_steg_msg_size(const Steg *steg)
{
  size_t maxMsgSize;
  StegStatus status = steg->fns->max_msg_size(steg, &maxMsgSize);
  if (status != STEG_OK) {
    fatal("cannot get max steg msg size for %s", steg_id(steg));
  }
  return maxMsgSize;
}


/** Create a new Steg object from the PPM image stored in path with
 *  its id set to the basename of path (without the extension).  It is
 *  the caller's responsibility to call steg->fns->free() on the
 *  returned steg object.
 */
const Steg *
new_steg_from_file(const char *path)
{
  TRACE("<-new_steg_from_file() %ld: path= %s", (long)getpid(), path);
  size_t nBytes;
  const Byte *bytes = (const Byte *)read_file(path, &nBytes);
  TRACE("new_steg_from_file() %ld: read %lu bytes from %s",
        (long)getpid(), nBytes, path);
  const char *lastSlashP = strrchr(path, '/');
  const char *lastDotP = strrchr(path, '.');
  const char *idBeginP = (lastSlashP) ? (lastSlashP + 1) :  path;
  const char *idEndP = (lastDotP) ? lastDotP : path + strlen(path);
  const size_t idLen = idEndP - idBeginP;
  char id[idLen + 1];
  strncpy(id, idBeginP, idLen); id[idLen] = '\0';
  Steg *steg;
  StegStatus status = new_steg(id, bytes, nBytes, &steg);
  free((void *)bytes); //steg has its own copy
  if (status != STEG_OK) fatal("%s: %s", path, steg_status(status));
  TRACE("<-new_steg_from_file() %ld: read steg %s from path %s",
        (long)getpid(), steg_id(steg), path);
  return steg;
}
