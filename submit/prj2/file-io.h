#ifndef FILE_IO_
#define FILE_IO_

#include <stddef.h>

/** Return contents of path.  Returned content is dynamically
 *  allocated with size return'd in *sizeP.  Terminates program with
 *  message on error.  The caller must free() the return'd value when
 *  no longer required.
 */
const char *read_file(const char *path, size_t *sizeP);

/** Write/replace contents of file specified by path with bytes[size].
 *  Terminates program with message on error.
 */
void write_file(const char *path, const char bytes[], size_t size);

#endif //ifndef FILE_IO_

