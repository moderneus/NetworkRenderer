#include "utils/FileHelper.hpp"

#include <fstream>

#include "fmt/color.h"
#include "fmt/core.h"

std::vector<char> utils::file::FileHelper::ReadFile(const std::string &path) {
  std::ifstream file(path, std::ios::ate | std::ios::binary);

  if (!file.is_open())
    fmt::print(fmt::fg(fmt::color::dark_red),
        "[FILE] ERROR: Failed to open the file by path: {}\n", path);

  std::size_t bufferSize = static_cast<std::size_t>(file.tellg());
  std::vector<char> buffer(bufferSize);

  file.seekg(0);
  file.read(buffer.data(), bufferSize);
  file.close();

  return buffer;
}
