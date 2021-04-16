//
// Created by Andreas on 2021-03-24.
//

#include <iostream>
#include <filesystem>
#include <algorithm>

#include "FileExplorer.h"

using namespace std;

FileExplorer::FileExplorer():
    filter("(.[a-zA-Z0-9]+)")
{
    setCurrentDir("..");
}

void FileExplorer::listDir() {
    filesystem::path dirPath(currentDir.absolutePath);

    if (!fileEntryList.empty()) {
        fileEntryList.clear();
    }

    // Add parent.
    filesystem::path parentPath = filesystem::absolute(dirPath).parent_path();
    if (!(parentPath == dirPath.root_path())) {
        FileEntry parent = {"..", parentPath.string(), true};
        fileEntryList.push_back(parent);
    }

    // Add files in currentDir to list.
    for (const auto& entry : filesystem::directory_iterator(dirPath)) {
        if (regex_match(entry.path().extension().string(), filter) || filesystem::is_directory(entry.path())) {
            FileEntry fe = {entry.path().filename().string(),
                            filesystem::absolute(entry.path()).string(),
                            filesystem::is_directory(entry.path())
            };
            fileEntryList.push_back(fe);
        }
    }

    // Finally sort the list.
    auto comparator = [](const FileEntry& lhs, const FileEntry& rhs) -> bool {
        return lhs.filename.compare(rhs.filename) < 0;
    };
    sort(fileEntryList.begin(), fileEntryList.end(), comparator);
}

void FileExplorer::setCurrentDir(std::string _currentDir) {
    filesystem::path dirPath(_currentDir);

    // Check if path is valid.
    if (!filesystem::exists(dirPath)) {
        NON_FATAL_ERROR("CurrentDir was not set because the argument is not a path.");
        return;
    }

    // Check if path is a directory.
    if (!filesystem::is_directory(dirPath)) {
        NON_FATAL_ERROR("CurrentDir was not set because path did not lead to a directory.");
        return;
    }
    FileEntry fe = {dirPath.filename().string(), filesystem::absolute(dirPath).string(), true};
    currentDir = fe;

    listDir();
}

void FileExplorer::setFilter(const std::string& _filter) {
    regex r;
    try {
        r = _filter;
    } catch (regex_error& e) {
        NON_FATAL_ERROR("Filter was not set because the argument was not valid regex.");
        return;
    }

    filter = std::move(r);

    listDir();
}

const FileExplorer::FileEntry& FileExplorer::getCurrentDir() const {
    return currentDir;
}

const std::vector<FileExplorer::FileEntry>& FileExplorer::getDirContents() const {
    return fileEntryList;
}

void FileExplorer::moveTo(const FileEntry& dir) {
    setCurrentDir(dir.absolutePath);
}