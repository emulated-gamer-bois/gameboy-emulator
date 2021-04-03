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

    # Build and link executable.
    add_executable ( ${PROJECT_NAME}
        main.cpp
        ${SHADERS}
        Application.h
        Application.cpp
            helpers/AppTimer.h
            helpers/AppTimer.cpp
        FileExplorer.h
        FileExplorer.cpp
        Gui.cpp
        Gui.h
        Keybinds.cpp
        Keybinds.h
        AppSettings.h
            helpers/ErrorReport.h
            )

    target_link_libraries ( ${PROJECT_NAME} gameboy IO )
    config_build_output()
endif()