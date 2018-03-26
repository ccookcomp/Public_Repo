#include "cmd-args.h"

#include <stdbool.h>
#include <stdio.h>

void
out_cmd_args(FILE *out, const CmdArgs *cmdArgs) {
  bool isHide = cmdArgs->cmd == HIDE_CMD;
  fprintf(out, "cmd = %s,\n", (isHide) ? "hide" : "unhide");
  fprintf(out, "msgPath = %s\n", cmdArgs->msgPath);
  fprintf(out, "nImgPaths = %d\n", cmdArgs->nImgPaths);
  fprintf(out, "imgPaths = [\n");
  for (int i = 0; i < cmdArgs->nImgPaths; i++) {
    fprintf(out, "  %s\n", cmdArgs->imgPaths[i]);
  }
  if (isHide) {
    fprintf(out, "destDir = %s\n", cmdArgs->destDir);
  }
}

