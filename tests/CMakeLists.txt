cmake_minimum_required (VERSION 3.2)

project( tests )

add_subdirectory(gtest)
enable_testing ()
if (WIN32)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
endif ()

# Find pthread library
find_package (Threads REQUIRED)

link_directories (
        ${GOOGLETEST_LIBRARY}
)

link_libraries (
        gtest
        gtest_main)

add_executable(tests
        tests.cpp
        cpu_test.cpp
        register_pair_test.cpp

        ../src/gameboy/cpu.cpp
        ../src/gameboy/cpu.h
        ../src/gameboy/register_pair.h)
