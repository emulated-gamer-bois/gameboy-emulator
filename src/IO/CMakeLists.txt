cmake_minimum_required ( VERSION 3.0.2 )

project ( IO )

# External libraries which need to be built.
add_subdirectory(${CMAKE_SOURCE_DIR}/external_src/imgui-1.81 ${CMAKE_CURRENT_BINARY_DIR}/imgui)
set_target_properties(imgui PROPERTIES FOLDER external)

set(glm_DIR "${CMAKE_SOURCE_DIR}/external/glm")
set(OpenGL_GL_PREFERENCE "GLVND")

set(OPENAL_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/external_src/openal-soft/include")
if(WIN32)
    set(OPENAL_LIBRARY "${CMAKE_SOURCE_DIR}/external/openal/OpenAL32.lib")
endif()

find_package ( OpenAL REQUIRED )
find_package ( SDL2   REQUIRED)
find_package ( glm    REQUIRED)
find_package ( GLEW   REQUIRED)
find_package ( OpenGL REQUIRED)

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
        Controller.h
        imgui_impl/imgui_impl_opengl3.cpp
        imgui_impl/imgui_impl_opengl3.h
        imgui_impl/imgui_impl_sdl.cpp
        imgui_impl/imgui_impl_sdl.h
        )

target_include_directories( IO
        PUBLIC
        #${CMAKE_SOURCE_DIR}/external_src/stb-master
        #${CMAKE_SOURCE_DIR}/external_src/tinyobjloader-1.0.6
        ${SDL2_INCLUDE_DIRS}
        ${GLM_INCLUDE_DIRS}
        ${GLEW_INCLUDE_DIRS}
        ${OPENGL_INCLUDE_DIR}
        ${OPENAL_INCLUDE_DIR}
        )

if(WIN32)
    target_link_libraries ( ${PROJECT_NAME}
            PUBLIC
            imgui
            ${SDL2_LIBRARIES}
            ${GLEW_LIBRARIES}
            ${OPENGL_LIBRARY}
            ${OPENAL_LIBRARY}
            )
else()
    target_link_libraries ( ${PROJECT_NAME}
            PUBLIC
            imgui
            SDL2::SDL2
            ${SDL2_LIBRARIES}
            ${GLEW_LIBRARIES}
            ${OPENGL_LIBRARY}
            ${OPENAL_LIBRARY}
            )
endif()