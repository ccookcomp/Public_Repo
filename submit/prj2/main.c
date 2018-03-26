#include "cmd-args.h"
#include "multi-steg.h"

#include "errors.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

enum { MAX_N_WORKERS = 16 };

static void usage(const char *argv[]) {
  fatal("usage: %s \\\n"
        "           hide   N_WORKERS IMG_PATH... SRC_MSG_PATH DEST_DIR |\n"
        "           unhide N_WORKERS IMG_PATH... DEST_MSG_PATH", argv[0]);
}

static bool
path_exists(const char *path, bool isDir)
{
  struct stat statBuf;
  if (stat(path, &statBuf) < 0) return false;
  return (isDir) ? S_ISDIR(statBuf.st_mode) : S_ISREG(statBuf.st_mode);
}

int
main(int argc, const char *argv[])
{
  if (argc < 5) usage(argv);
  CmdArgs cmd;
  cmd.cmd = strcmp(argv[1], "hide")==0 ? HIDE_CMD :
            strcmp(argv[1], "unhide")==0 ? UNHIDE_CMD : BAD_CMD;
  if (cmd.cmd == BAD_CMD) {
    error("bad command %s", argv[1]);
    usage(argv);
  }
  bool isHide = (cmd.cmd == HIDE_CMD);
  char *p;
  cmd.nWorkers = strtol(argv[2], &p, 10);
  if (cmd.nWorkers < 1 || cmd.nWorkers > MAX_N_WORKERS || *p != '\0') {
    error("N_WORKERS must be in [1, %d]", MAX_N_WORKERS);
    usage(argv);
  }
  int msgPathIndex = (isHide) ? argc - 2 : argc - 1;
  cmd.msgPath = argv[msgPathIndex];
  enum { imgsStartIndex = 3 };
  if (msgPathIndex == imgsStartIndex) {
    error("one or more image paths must be specified");
    usage(argv);
  }
  cmd.imgPaths = &argv[imgsStartIndex];
  cmd.nImgPaths = msgPathIndex - imgsStartIndex;
  int endCheckFilesIndex = (isHide) ? msgPathIndex + 1 : msgPathIndex;
  for (int i = imgsStartIndex; i < endCheckFilesIndex; i++) {
    if (!path_exists(argv[i], false)) {
      error("%s is not a file", argv[i]);
      usage(argv);
    }
  }
  if (isHide) cmd.destDir = argv[argc - 1];
  if (isHide && !path_exists(cmd.destDir, true)) {
    error("%s is not a directory", cmd.destDir);
    usage(argv);
  }
  //out_cmd_args(stdout, &cmd);
  do_multi_steg(&cmd);
  return 0;
}

