#pragma once

#include "absl/base/optimization.h"  // IWYU pragma: keep
#include "absl/status/status.h"      // IWYU pragma: keep
#include "absl/status/statusor.h"    // IWYU pragma: keep

#define RETURN_IF_ERROR(v)                   \
  do {                                       \
    const absl::Status _status = (v);        \
    if (ABSL_PREDICT_FALSE(!_status.ok())) { \
      return _status;                        \
    }                                        \
  } while (0)

#define STATUS_MACROS_IMPL_CONCAT_INNER(x, y) x##y
#define STATUS_MACROS_IMPL_CONCAT(x, y)       STATUS_MACROS_IMPL_CONCAT_INNER(x, y)

#define ASSIGN_OR_RETURN_IMPL(statusor, lhs, rexpr) \
  auto statusor = (rexpr);                          \
  if (ABSL_PREDICT_FALSE(!statusor.ok())) {         \
    return std::move(statusor).status();            \
  }                                                 \
  lhs = std::move(statusor).value()

#define ASSIGN_OR_RETURN(lhs, rexpr)                                           \
  ASSIGN_OR_RETURN_IMPL(STATUS_MACROS_IMPL_CONCAT(_status_or_value, __LINE__), \
                        lhs, rexpr)

#ifdef NDEBUG
#define ASSUME(expr) __builtin_assume((expr))
#else
#define ASSUME(expr)                                        \
  if (!(expr)) {                                            \
    internal::assumption_failed(#expr, __FILE__, __LINE__); \
  }                                                         \
  __builtin_assume((expr))
#endif

namespace internal {

[[noreturn]] void assumption_failed(const char* expr, const char* file,
                                    int line);

}
