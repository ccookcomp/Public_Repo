#include "steg-server.h"

#include "errors.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/** main() driver program for stegd server */
int
main(int argc, const char *argv[])
{
  if (argc == 3 ? strcmp(argv[1], "-q") : argc != 2) {
    fatal("usage: %s [-q] SERVER_DIR", argv[0]);
  }
  bool isQuiet = (argc == 3);
  const char *serverDir = isQuiet ? argv[2] : argv[1];
  const StegServerOpts opts = { .isQuiet = isQuiet };
  pid_t pid = steg_serve(serverDir, &opts);
  if (pid < 0) exit(1);
  fprintf(stdout, "%ld\n", (long)pid);
  return 0;
}

