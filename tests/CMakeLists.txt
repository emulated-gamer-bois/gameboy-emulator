cmake_minimum_required (VERSION 3.2)

project( tests )
add_subdirectory(gtest)
enable_testing ()

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
        mmu_test.cpp
        ppu_test.cpp)

target_link_libraries(${PROJECT_NAME} gameboy )
