#pragma once

#include <string>
#include <vector>
#include <optional>

class FileHelper {
public:
    struct FileEntry {
        std::string filename;
        std::string absolutePath;
        bool isDir;
    };

    static std::string openDialog();
    static std::optional<std::vector<FileEntry>> getDirContents(std::string _dirPath, std::string _filter = ".[a-z1-9]*");
    static std::optional<std::vector<FileEntry>> getParentDir(std::string _dirPath);
};
