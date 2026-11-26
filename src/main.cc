#include <iostream>

#include "src/huffman.h"

namespace {

[[nodiscard]] int Usage(char* argv[]) {
  std::cerr << "Usage: " << argv[0]
            << " --(de)?compress <input_file> [<output_file>]" << std::endl;
  return -1;
}

}  // namespace

int main(int argc, char* argv[]) {
  if (argc != 3 && argc != 4) return Usage(argv);

  std::string_view mode = argv[1];
  std::string_view input = argv[2];
  std::optional<std::string_view> output =
      argc == 4 ? std::optional(argv[3]) : std::nullopt;

  absl::Status result;
  if (mode == "--compress") {
    Huffman compressor;
    result = compressor.CompressFile(input, output.value_or("archive9.bhm"));
  } else {
    return Usage(argv);
  }

  if (!result.ok()) {
    std::cerr << result << std::endl;
    return -1;
  }

  return 0;
}
