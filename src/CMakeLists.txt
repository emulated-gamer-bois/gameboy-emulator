cmake_minimum_required ( VERSION 3.0.2 )

project ( project )

add_subdirectory( gameboy )

# Excludes graphics code if tests are run in travis
if(NOT TRAVIS)
    # Find *all* shaders.
    file(GLOB_RECURSE shaders
        "shaders/*.vert"
        "shaders/*.frag"
    )

    # Separate filter for shaders.
    source_group("shaders" FILES ${SHADERS})

    add_subdirectory( IO )
    add_subdirectory( helpers ) # No need in tests?

    # Build and link executable.
    add_executable ( ${PROJECT_NAME}
        main.cpp
        ${SHADERS}
            application/Application.h
            application/Application.cpp
            IO/FileExplorer.h
            IO/FileExplorer.cpp
            IO/Gui.cpp
            IO/Gui.h
            application/Keybinds.cpp
            application/Keybinds.h
            application/AppSettings.h
            View.cpp
            IO/View.h
            IO/Controller.cpp
            IO/Controller.h
            application/State.h)

    target_link_libraries ( ${PROJECT_NAME} gameboy IO helpers )
    config_build_output()
endif()