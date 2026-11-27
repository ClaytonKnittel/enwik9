#include "src/util.h"

#include <cstdio>

namespace internal {

void assumption_failed(const char* expr, const char* file, int line) {
  fprintf(stderr, "%s: %d: Assumption failed: '%s'\n", file, line, expr);
  abort();
}

}  // namespace internal