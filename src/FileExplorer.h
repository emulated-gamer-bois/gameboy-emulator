#pragma once

#include <string>
#include <vector>
#include <regex>

struct FileEntry {
    std::string filename;
    std::string absolutePath;
    bool isDir;
};

class FileExplorer {
private:
    std::string currentDir;
    std::regex filter;
    std::vector<FileEntry> fileEntryList;

    void listDir();

public:
    void setCurrentDir(std::string currentDir);
    void setFilter(const std::string& filter);
    [[nodiscard]] const std::string& getCurrentDir() const;
    [[nodiscard]] const std::vector<FileEntry>& getDirContents() const;

    void moveTo(const FileEntry& dir);
    void moveBack();
};
