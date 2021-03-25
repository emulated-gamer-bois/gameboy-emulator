//
// Created by Andreas on 2021-03-24.
//

#include <iostream>
#include <filesystem>
#include <algorithm>

#include "FileExplorer.h"

using namespace std;

void FileExplorer::listDir() {
    filesystem::path dirPath(currentDir);

    if (!fileEntryList.empty()) {
        fileEntryList.clear();
    }

    // Add parent.
    FileEntry parent = {"..", filesystem::absolute(dirPath).parent_path().string(), true};
    fileEntryList.push_back(parent);

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

void FileExplorer::setCurrentDir(std::string currentDir) {
    // Check if path is valid.
    if (!filesystem::exists(currentDir)) {
        cerr << "FileHelper ERROR: Invalid path" << endl;
        return;
    }

    // Check if path is a directory.
    if (!filesystem::is_directory(currentDir)) {
        cerr << "FileHelper ERROR: Path is not a directory" << endl;
        return;
    }

    this->currentDir = currentDir;
}

void FileExplorer::setFilter(const std::string& filter) {
    regex r;
    try {
        r = filter;
    } catch (regex_error& e) {
        cerr << "FileHelper ERROR: Filter is not valid regex" << endl;
        return;
    }

    this->filter = std::move(r);
}

const std::string& FileExplorer::getCurrentDir() const {
    return currentDir;
}

const std::vector<FileEntry>& FileExplorer::getDirContents() const {
    return fileEntryList;
}

void FileExplorer::moveTo(const FileEntry& dir) {
    if (!dir.isDir) {
        cerr << "FileHelper ERROR: Path is not a directory" << endl;
        return;
    }

    currentDir = dir.absolutePath;
}