cmake_minimum_required ( VERSION 3.0.2 )

project ( IO )

set(OPENAL_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/external_src/openal-soft/include")
if(WIN32)
 set(OPENAL_LIBRARY "${CMAKE_SOURCE_DIR}/external/openal/OpenAL32.lib")
endif()

find_package ( OpenAL REQUIRED )

# Find *all* shaders.
file(GLOB_RECURSE shaders
        "shaders/*.vert"
        "shaders/*.frag"
        )

# Separate filter for shaders.
source_group("shaders" FILES ${SHADERS})

add_library( IO
        ${SHADERS}
        AudioController.cpp
        AudioController.h
        Palette.h
        PaletteHandler.h
        PaletteHandler.cpp
        RenderView.cpp
        RenderView.h
        FileExplorer.h
        FileExplorer.cpp
        GuiView.cpp
        GuiView.h
        Controller.cpp
        Controller.h)

target_include_directories ( ${PROJECT_NAME}
        PUBLIC
        ${OPENAL_INCLUDE_DIR} )

target_link_libraries ( ${PROJECT_NAME} labhelper ${OPENAL_LIBRARY} )