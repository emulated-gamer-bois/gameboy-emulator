cmake_minimum_required (VERSION 3.2)
add_subdirectory(gtest)
set (GOOGLETEST_ROOT ${CMAKE_SOURCE_DIR}/googletest)
set (GOOGLETEST_INCLUDE ${CMAKE_SOURCE_DIR}/googletest/include)
set (GOOGLETEST_BINARY_DIR ${CMAKE_SOURCE_DIR}/googletest)
set (GOOGLETEST_LIBRARY ${CMAKE_SOURCE_DIR}/googletest)
enable_testing ()
# Use booleval's include directories + test include directories
include_directories (
        ${GOOGLETEST_INCLUDE}
)

# Find pthread library
find_package (Threads REQUIRED)

link_directories (
        ${GOOGLETEST_LIBRARY}
)

# Link against GoogleTest, libbooleval and pthread.
# Pthread is required by GoogleTest
link_libraries (
        gtest
        gtest_main
)
add_executable(tests tests.cpp)
