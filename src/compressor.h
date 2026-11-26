#pragma once

#include <string_view>

#include "absl/status/status.h"
#include "absl/status/statusor.h"

class Compressor {
 public:
  virtual ~Compressor() = default;

  absl::Status CompressFile(std::string_view input, std::string_view output);

 protected:
  virtual absl::StatusOr<std::unique_ptr<std::istream>> Compress(
      std::string input) = 0;
};
