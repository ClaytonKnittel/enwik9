#pragma once

#include <ostream>

#include "absl/log/absl_check.h"

class Char {
  static constexpr uint32_t kMaxAscii = 0x7F;

  static constexpr bool IsLength2(uint8_t prefix) {
    ABSL_DCHECK_GT(prefix, kMaxAscii);
    return (prefix & 0xE0) == 0xC0;
  }
  static constexpr bool IsLength3(uint8_t prefix) {
    return (prefix & 0xF0) == 0xE0;
  }
  static constexpr bool IsLength4(uint8_t prefix) {
    return (prefix & 0xF8) == 0xF0;
  }

 public:
  static Char null() {
    return Char(0);
  }

  static Char ascii(char c) {
    ABSL_DCHECK_LE(c, kMaxAscii);
    return Char(c);
  }

  static Char ParseCharacter(absl::string_view char_str) {
    auto result = ParseString(char_str);
    ABSL_DCHECK_EQ(result.size(), 1);
    return result[0];
  }

  static std::vector<Char> ParseString(absl::string_view input);

  size_t size() const {
    if (encoding_ <= kMaxAscii) return 1;
    if (IsLength2(encoding_)) return 2;
    if (IsLength3(encoding_)) return 3;
    return 4;
  }

  friend bool operator==(const Char&, const Char&) = default;
  friend bool operator!=(const Char&, const Char&) = default;
  friend bool operator<=>(const Char&, const Char&) = default;

  template <typename H>
  friend H AbslHashValue(H h, const Char& c) {
    return H::combine(std::move(h), c.encoding_);
  }

 private:
  friend std::ostream& operator<<(std::ostream& out, const Char& c) {
    const char* begin = reinterpret_cast<const char*>(&c.encoding_);
    return out << std::string_view(begin, begin + c.size());
  }

  explicit Char(uint32_t encoding) : encoding_(encoding) {}

  uint32_t encoding_;
};
