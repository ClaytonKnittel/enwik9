
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
  static std::vector<Char> ParseString(absl::string_view input) {
    std::vector<Char> out;
    out.reserve(input.size());

    const auto to_u32 = [](char c) -> uint32_t {
      return static_cast<uint8_t>(c);
    };

    for (size_t i = 0; i < input.length(); ++i) {
      const auto prefix = static_cast<uint8_t>(input[i]);
      uint32_t c = prefix;
      if (prefix > kMaxAscii) {
        i++;
        ABSL_DCHECK_LT(i, input.length());
        c += to_u32(input[i]) << 8;

        if (!IsLength2(prefix)) {
          i++;
          ABSL_DCHECK_LT(i, input.length());
          c += to_u32(input[i]) << 16;

          if (!IsLength3(prefix)) {
            i++;
            ABSL_DCHECK(IsLength4(prefix));
            ABSL_DCHECK_LT(i, input.length());
            c += to_u32(input[i]) << 24;
          }
        }
      }

      out.push_back(Char{c});
    }

    return out;
  }

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