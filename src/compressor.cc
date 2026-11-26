#include "src/compressor.h"

#include <fstream>
#include <sstream>

#include "absl/status/status.h"
#include "absl/strings/str_cat.h"

#include "src/util.h"

namespace {

absl::StatusOr<std::string> ReadToString(std::string_view file_path) {
  std::filesystem::path path = file_path;
  std::ifstream file(path);
  if (!file) {
    return absl::NotFoundError(
        absl::StrCat("File \"", file_path, "\" not found"));
  }

  std::ostringstream str;
  str << file.rdbuf();
  return std::move(str).str();
}

absl::Status WriteToFile(std::istream& stream, std::string_view file_path) {
  std::filesystem::path path = file_path;
  std::ofstream file(path);
  if (!file) {
    return absl::AlreadyExistsError(
        absl::StrCat("Failed to open file \"", file_path, "\" for writing"));
  }

  if (!(file << stream.rdbuf())) {
    return absl::UnknownError(absl::StrCat("Failed to write to file"));
  }

  return absl::OkStatus();
}

}  // namespace

absl::Status Compressor::CompressFile(std::string_view input,
                                      std::string_view output) {
  ASSIGN_OR_RETURN(std::string file_contents, ReadToString(input));
  ASSIGN_OR_RETURN(std::unique_ptr<std::istream> compressed_text,
                   Compress(std::move(file_contents)));
  return WriteToFile(*compressed_text, output);
}
