#ifndef CMD_ARGS_H_
#define CMD_ARGS_H_

#include <stdio.h>

typedef enum { HIDE_CMD, UNHIDE_CMD, BAD_CMD } Cmd;

/** Structure which captures command-line arguments specifying
 *  multi-steg operation.
 */
typedef struct {
  Cmd cmd;                 /** specifies HIDE_CMD or UNHIDE_CMD */
  int nWorkers;            /** # of child worker processes */
  const char *msgPath;     /** Path to msg file for hide (src), unhide (dest) */
  int nImgPaths;           /** # of paths in imgPaths */
  const char **imgPaths;   /** Paths to image files */
  union {                  /** pull apart args needed only for a specific cmd */
    struct {               /** valid iff cmd == HIDE_CMD */
      const char *destDir; /** Directory where NN*.ppm output images created */
    };
  };
} CmdArgs;

/** Output cmdArgs on out. */
void out_cmd_args(FILE *out, const CmdArgs *cmdArgs);


#endif //ifndef CMD_ARGS_H_

