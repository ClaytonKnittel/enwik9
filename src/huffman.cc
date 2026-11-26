#include "src/huffman.h"

#include <istream>
#include <memory>
#include <sstream>

#include "absl/status/statusor.h"

absl::StatusOr<std::unique_ptr<std::istream>> Huffman::Compress(
    std::string input) {
  return std::make_unique<std::istringstream>(std::move(input));
}
