#include "src/char.h"

std::vector<Char> Char::ParseString(absl::string_view input) {
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