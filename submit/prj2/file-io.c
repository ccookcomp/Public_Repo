#include "file-io.h"

#include "errors.h"
#include "memalloc.h"

#include <stdbool.h>
#include <stdio.h>

/** Return dynamically allocated content of f with size *nReadP.
 *  Makes a single sequential pass over f, hence it can be used
 *  for non-seekable f.
 */
static const char *
read_file_content(FILE *f, size_t *nReadP)
{
  enum { INIT_SIZE = 1<<16 };
  size_t nToRead = INIT_SIZE;   //# of chars to read on next attempt
  char *content = NULL;
  size_t contentSize = 0;
  size_t nRead = 0;  //# of chars read so far
  while (true) {
    content = reallocChk(content, contentSize += nToRead);
    int n = fread(content + nRead, 1, nToRead, f);
    nRead += n;
    if (n < nToRead) break;
    nToRead *= 2;
  }
  if (!feof(f)) fatal("read_file_content():");
  content = reallocChk(content, nRead);
  *nReadP = nRead;
  return content;
}

/** Return contents of path.  Returned content is dynamically
 *  allocated with size return'd in *sizeP.  Terminates program with
 *  message on error.  The caller must free() the return'd value when
 *  no longer required.
 */
const char *
read_file(const char *path, size_t *sizeP)
{
  FILE *f = fopen(path, "r");
  if (!f) fatal("cannot open %s:", path);
  const char *content = read_file_content(f, sizeP);
  if (fclose(f) != 0) fatal("cannot close %s:", path);
  return content;
}


/** Write/replace contents of file specified by path with bytes[size].
 *  Terminates program with message on error.
 */
void
write_file(const char *path, const char bytes[], size_t size)
{
  FILE *f = fopen(path, "w");
  if (!f) fatal("cannot open %s:", path);
  if (fwrite(bytes, size, 1, f) != 1) fatal("cannot write %s:", path);
  if (fclose(f) != 0) fatal("cannot close %s:", path);
}

