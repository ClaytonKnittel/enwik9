#include "src/huffman.h"

#include <istream>
#include <memory>
#include <sstream>
#include <string_view>

#include "absl/container/flat_hash_map.h"
#include "absl/status/statusor.h"

#include "src/char.h"

namespace {

absl::flat_hash_map<Char, size_t> CharFrequency(std::string_view stream) {
  absl::flat_hash_map<Char, size_t> freq;
  for (Char c : Char::ParseString(stream)) {
    freq[c]++;
  }
  return freq;
}

}  // namespace

absl::StatusOr<std::unique_ptr<std::istream>> Huffman::Compress(
    std::string input) {
  absl::flat_hash_map<Char, size_t> chars = CharFrequency(input);

  std::ostringstream out;
  out << "Total unique chars: " << chars.size() << std::endl;

  std::vector<std::pair<Char, size_t>> sorted_chars(chars.begin(), chars.end());
  absl::c_sort(sorted_chars, [](const auto& lhs, const auto& rhs) {
    return lhs.second > rhs.second;
  });
  for (auto [c, count] : sorted_chars) {
    out << c << " " << count << std::endl;
  }

  return std::make_unique<std::istringstream>(std::move(out).str());
}
