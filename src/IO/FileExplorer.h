#pragma once

#include <vector> //vector
#include <regex> // regex

class FileExplorer {
public:
    struct FileEntry {
        std::string filename;
        std::string absolutePath;
        bool isDir;
    };

    FileExplorer();

    void setCurrentDir(std::string currentDir);
    void setFilter(const std::string& filter);
    [[nodiscard]] const FileEntry& getCurrentDir() const;
    [[nodiscard]] const std::vector<FileEntry>& getDirContents() const;

    void moveTo(const FileEntry& dir);

private:
    FileEntry currentDir;
    std::regex filter;
    std::vector<FileEntry> fileEntryList;

    void listDir();
};
