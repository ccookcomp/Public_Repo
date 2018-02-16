//program includes
#include "steg.h"

//cs551 lib includes
#include "errors.h"
#include "memalloc.h"

//system includes
#include <stdio.h>
#include <stdlib.h>


/** Read filename returning its content in dynamically allocated memory,
 *  with its total data length in *lenP.
 */
static unsigned char *
read_file(const char *filename, int *lenP)
{
  FILE *f = fopen(filename, "r");
  if (f == NULL) {
    fatal("cannot open file %s:", filename);
  }
  if (fseek(f, 0, SEEK_END) < 0) {
    fatal("cannot seek on file %s:", filename);
  }
  int len = ftell(f);
  if (len < 0) {
    fatal("cannot tell file position on file %s:", filename);
  }
  rewind(f);
  unsigned char *data = mallocChk(len);
  if (fread(data, 1, len, f) != len) {
    fatal("cannot read file %s:", filename);
  }
  if (fclose(f) != 0) {
    fatal("cannot close file %s:", filename);
  }
  *lenP = len;
  return data;
}

static void
outBytes(FILE *out, Byte bytes[], int nBytes)
{
  for (int i = 0; i < nBytes; i++) {
    fprintf(out, "%c", bytes[i]);
  }
}

static void
verifyStatus(const char *fileName, StegStatus status)
{
  if (status != STEG_OK) fatal("%s: %s", fileName, steg_status(status));
}

int
main(int argc, const char *argv[]) {
  FILE *out = stdout;
  if (argc < 2 || argc > 3) {
    fatal("usage: %s PPM_IMAGE_FILE [MSG]", argv[0]);
  }
  const char *fileName = argv[1];
  int nBytes;
  const Byte *bytes = read_file(fileName, &nBytes);
  Steg *steg;
  StegStatus status = new_steg(fileName, bytes, nBytes, &steg);
  free((Byte *)bytes);
  verifyStatus(fileName, status);
  if (argc == 3) { //hide
    const char *msg = argv[2];
    Byte out[nBytes];
    status = steg->fns->hide(steg, msg, out);
    verifyStatus(fileName, status);
    outBytes(stdout, out, nBytes);
  }
  else { //unhide
    size_t maxMsgSize;
    status = steg->fns->max_msg_size(steg, &maxMsgSize);
    verifyStatus(fileName, status);
    char msg[maxMsgSize];
	status = steg->fns->unhide(steg, msg);
    verifyStatus(fileName, status);
    fprintf(out, "%s\n", msg);
  }
  status = steg->fns->free(steg);
  verifyStatus(fileName, status);
  return 0;
}
