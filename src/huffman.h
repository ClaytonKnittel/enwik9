#pragma once

#include <istream>
#include <memory>

#include "absl/status/statusor.h"

#include "src/compressor.h"

class Huffman : public Compressor {
 public:
 protected:
  absl::StatusOr<std::unique_ptr<std::istream>> Compress(
      std::string input) override;
};
