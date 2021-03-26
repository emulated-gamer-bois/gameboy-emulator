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

    set(OPENAL_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/external_src/openal-soft/include")
    if(WIN32)
        set(OPENAL_LIBRARY "${CMAKE_SOURCE_DIR}/external/openal/OpenAL32.lib")
    endif()

    find_package ( OpenAL REQUIRED )

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
        AudioController.h
        AudioController.cpp)

    target_include_directories ( ${PROJECT_NAME}
            PUBLIC
            ${OPENAL_INCLUDE_DIR} )

    target_link_libraries ( ${PROJECT_NAME} labhelper gameboy ${OPENAL_LIBRARY} )
    config_build_output()
endif()