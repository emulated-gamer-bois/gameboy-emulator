/**
 * File contains functions used to check for errors.
 */

#pragma once

#include <iostream>

#define FATAL_ERROR(log) { ErrorReport::fatalError(log, __LINE__, __FILE__); }
#define NON_FATAL_ERROR(log) { ErrorReport::nonFatalError(log, __LINE__, __FILE__); }
#define INFO_LOG(log) { ErrorReport::infoLog(log); }

// Uncomment for printing non fatal errors
#define DEBUG_PRINT

namespace ErrorReport {
    static void fatalError(const std::string& errorLog, int line, const char* file) {
        std::cerr << "--FATAL ERROR--" << std::endl;
        std::cerr << "FILE: " << file << std::endl;
        std::cerr << "LINE: " << line << std::endl;
        std::cerr << "LOG:  " << errorLog << std::endl;

        abort();
    }

    static void nonFatalError(const std::string& errorLog, int line, const char* file) {
#ifdef DEBUG_PRINT
        std::cout << "--NON FATAL ERROR--" << std::endl;
        std::cout << "FILE: " << file << std::endl;
        std::cout << "LINE: " << line << std::endl;
        std::cout << "LOG:  " << errorLog << std::endl;
#endif
    }

    static void infoLog(const std::string& log) {
#ifdef DEBUG_PRINT
        std::cout << "INFO: " << log << std::endl;
#endif
    }
};
