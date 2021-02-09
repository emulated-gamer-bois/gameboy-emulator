cmake_minimum_required (VERSION 3.2)
add_subdirectory(gtest)
enable_testing ()

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
