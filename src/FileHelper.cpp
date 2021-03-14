#include "FileHelper.h"

#include <iostream>
#include <iomanip>
#include <filesystem>

namespace fs = std::filesystem;

std::string FileHelper::openDialog() {
    std::string path = "../";
    for (const auto & entry : fs::directory_iterator(path))
        std::cout << entry.path() << std::endl;

    return "";
}