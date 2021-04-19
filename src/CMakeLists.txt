cmake_minimum_required ( VERSION 3.0.2 )

project ( project )

add_subdirectory( gameboy )
add_subdirectory( helpers )

# Excludes graphics code if tests are run in travis
if(NOT TRAVIS)
    add_subdirectory( IO )
    add_subdirectory( application )

    # Build and link executable.
    add_executable ( ${PROJECT_NAME} main.cpp)

    target_link_libraries ( ${PROJECT_NAME} gameboy IO helpers application )
    config_build_output()
endif()