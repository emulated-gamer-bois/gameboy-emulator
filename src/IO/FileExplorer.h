/**
 * A helper class to navigate the filesystem.
 */

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

    /**
     * Sets the currently visited directory to currentDir.
     *
     * @param currentDir
     */
    void setCurrentDir(std::string currentDir);

    /**
     * Sets the filter for what files to keep track of in the current directory.
     *
     * @param filter
     */
    void setFilter(const std::string& filter);

    /**
     * Returns the file entry of the current directory.
     *
     * @return File entry of the current directory.
     */
    [[nodiscard]] const FileEntry& getCurrentDir() const;

    /**
     * Returns a list of file entries for all files in the current directory that is not filtered out by filter.
     *
     * @return List of file entries for all files in the current directory that is not filtered out by filter.
     */
    [[nodiscard]] const std::vector<FileEntry>& getDirContents() const;

    /**
     * Same as setCurrent dir but takes a fileEntry instead of a string.
     *
     * @param File entry of current dir.
     */
    void moveTo(const FileEntry& dir);

private:
    FileEntry currentDir;
    std::regex filter;
    std::vector<FileEntry> fileEntryList;

    void listDir();
};
