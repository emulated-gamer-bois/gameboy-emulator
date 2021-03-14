#pragma once

#include <string>
#include <vector>
#include <optional>

class FileHelper {
public:
    static std::string openDialog();
    static std::optional<std::vector<std::string>> getDirContents(std::string _dirPath, std::string _filter = ".[a-z1-9]*");
    static std::optional<std::vector<std::string>> getParentDir(std::string _dirPath);
};
