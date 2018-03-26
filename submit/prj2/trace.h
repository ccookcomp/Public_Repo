/** Supports printf() style TRACE() statements in code.  They are
 *  activated only if the DO_TRACE macro is defined before this
 *  file in #include'd into the program.  Can be included multiple
 *  timed into the same compilation unit.
 */

#undef TRACE
#ifdef DO_TRACE

#include <stdio.h>

//first arg must be a literal string, not a var
#define TRACE(fmt, ...) do {                \
  fprintf(stderr, fmt "\n", ##__VA_ARGS__); \
} while (0)

#else
#define TRACE(...)
#endif
