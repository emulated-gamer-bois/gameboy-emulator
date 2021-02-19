cmake_minimum_required ( VERSION 3.0.2 )
project( gameboy )

# Must be included each time a library is made
if (WIN32)
    string(REPLACE "/MD" /MT CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE})
    string(REPLACE "/MDd" /MTd CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})
endif ()

add_library( ${PROJECT_NAME} cpu.cpp cpu.h register_pair.h mmu.h mmu.cpp gameboy.h gameboy.cpp)
