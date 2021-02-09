cmake_minimum_required (VERSION 3.2)
add_subdirectory(gtest)
set (GOOGLETEST_ROOT ${CMAKE_SOURCE_DIR}/googletest)
set (GOOGLETEST_INCLUDE ${CMAKE_SOURCE_DIR}/googletest/include)
set (GOOGLETEST_BINARY_DIR ${CMAKE_SOURCE_DIR}/googletest)
set (GOOGLETEST_LIBRARY ${CMAKE_SOURCE_DIR}/googletest)
enable_testing ()
include_directories (
        ${GOOGLETEST_INCLUDE}
)

# Find pthread library
find_package (Threads REQUIRED)

link_directories (
        ${GOOGLETEST_LIBRARY}
)

link_libraries (
        gtest
        gtest_main
)
add_executable(tests tests.cpp)
