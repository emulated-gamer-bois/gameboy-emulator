project(tests)
cmake_minimum_required(VERSION 3.9.2)

#Adds /tests as directory for tests, also adds GTest and Gmock.
include(gtest.cmake)
include(gmock.cmake)

SET(COVERAGE OFF CACHE BOOL "Coverage")
set(CMAKE_CXX_STANDARD 14)

add_executable(tests test.cpp)
find_package (Threads)
target_link_libraries(tests PRIVATE gtest ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(tests PRIVATE gmock ${CMAKE_THREAD_LIBS_INIT})
if (MSVC)
    target_compile_options(tests PRIVATE /std:c++17 /W4 /WX)

    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Ox")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Od")

    # VS 2017 removed tr1
    add_definitions(-DGTEST_HAS_TR1_TUPLE=0)
else()
    target_compile_options(tests PRIVATE -std=c++14 -g -Wall -Werror -Wextra -Wpedantic -Wconversion -Wswitch-default -Wswitch-enum -Wunreachable-code -Wwrite-strings -Wcast-align -Wshadow -Wundef)

    if (COVERAGE)
        target_compile_options(tests PRIVATE --coverage)
        target_link_libraries(tests PRIVATE --coverage)
    endif()
endif()

##Ending for tests.