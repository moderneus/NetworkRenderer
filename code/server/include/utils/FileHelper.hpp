#pragma once

#include <string>
#include <vector>

namespace utils::file {

class FileHelper {
public:
  std::vector<char> ReadFile(const std::string &path);
};

}; // namespace utils::file
