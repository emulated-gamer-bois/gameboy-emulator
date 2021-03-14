#include "FileHelper.h"

#include <iostream>
#include <filesystem>
#include <regex>
#include <optional>

using namespace std;

string FileHelper::openDialog() {
    return "";
}

optional<vector<FileHelper::FileEntry>> FileHelper::getDirContents(string _dirPath, string _filter) {
    auto entryPaths = vector<FileEntry>();
    filesystem::path dirPath(_dirPath);

    // Check if path is valid.
    if (!filesystem::exists(dirPath)) {
        cerr << "FileHelper ERROR: Invalid path" << endl;
        return nullopt;
    }

    // Check if path is a directory.
    if (!filesystem::is_directory(dirPath)) {
        cerr << "FileHelper ERROR: Path is not a directory" << endl;
        return nullopt;
    }

    // Check if regex is valid.
    regex filter;
    try {
        filter = _filter;
    } catch (regex_error& e) {
        cerr << "FileHelper ERROR: Filter is not valid regex" << endl;
        return nullopt;
    }

    for (const auto & entry : filesystem::directory_iterator(dirPath)) {
        if (regex_match(entry.path().extension().string(), filter) || filesystem::is_directory(entry.path())) {
            FileEntry fe = {entry.path().string(),
                            filesystem::absolute(entry.path()).string(),
                            filesystem::is_directory(entry.path())
            };
            entryPaths.push_back(fe);
        }
    }

    return entryPaths;
}