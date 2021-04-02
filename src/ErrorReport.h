#pragma once

#include <iostream>

#define FATAL_ERROR(log) { ErrorReport::fatalError(log, __LINE__, __FILE__); }
#define NON_FATAL_ERROR(log) { ErrorReport::nonFatalError(log, __LINE__, __FILE__); }

namespace ErrorReport {
    static void fatalError(const std::string& errorLog, int line, const char* file) {
        std::cerr << "--FATAL ERROR--" << std::endl;
        std::cerr << "FILE: " << file << std::endl;
        std::cerr << "LINE: " << line << std::endl;
        std::cerr << "LOG:  " << errorLog << std::endl;

        abort();
    }

    static void nonFatalError(const std::string& errorLog, int line, const char* file) {
        std::cout << "--NON FATAL ERROR--" << std::endl;
        std::cout << "FILE: " << file << std::endl;
        std::cout << "LINE: " << line << std::endl;
        std::cout << "LOG:  " << errorLog << std::endl;
    }
};
