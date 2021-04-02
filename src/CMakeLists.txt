cmake_minimum_required ( VERSION 3.0.2 )

project ( project )

add_subdirectory( gameboy )

# Excludes graphics code if tests are run in travis
if(NOT TRAVIS)
    # Find *all* shaders.
    file(GLOB_RECURSE SHADERS
        "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.vert"
        "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.frag"
    )
    # Separate filter for shaders.
    source_group("Shaders" FILES ${SHADERS})

    # Build and link executable.
    add_executable ( ${PROJECT_NAME}
        main.cpp
        ${SHADERS}
        RenderView.h
        RenderView.cpp
        Application.h
        Application.cpp
        Palette.h
        AppTimer.h
        AppTimer.cpp
        FileExplorer.h
        FileExplorer.cpp
        Gui.cpp
        Gui.h
        Keybinds.cpp
        Keybinds.h
        AppSettings.h
        ErrorReport.h
            )

    target_link_libraries ( ${PROJECT_NAME} labhelper gameboy )
    config_build_output()
endif()