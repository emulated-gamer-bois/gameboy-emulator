cmake_minimum_required (VERSION 3.2)

project( tests )
add_subdirectory(gtest)
enable_testing ()

link_directories ( ${GOOGLETEST_LIBRARY} )
link_libraries ( gtest gtest_main )

# Excludes audio_test.cpp from travis since no one will hear it
if(NOT TRAVIS)
    add_executable( tests
            tests.cpp
            cpu_test.cpp
            register_pair_test.cpp
            mmu_test.cpp
            ppu_test.cpp
            audio_test.cpp
            )
    target_link_libraries(${PROJECT_NAME} IO)
    target_link_libraries(${PROJECT_NAME} application)
    else()
    add_executable( tests
            tests.cpp
            cpu_test.cpp
            register_pair_test.cpp
            mmu_test.cpp
            ppu_test.cpp
            )
endif()

target_link_libraries ( ${PROJECT_NAME} gameboy )
