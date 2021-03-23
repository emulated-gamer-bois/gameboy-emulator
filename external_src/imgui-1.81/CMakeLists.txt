cmake_minimum_required(VERSION 3.0.2)

project(imgui)

# Build and link executable.
add_library(imgui
        ${CMAKE_CURRENT_SOURCE_DIR}/imconfig.h
        ${CMAKE_CURRENT_SOURCE_DIR}/imgui.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/imgui.h
        ${CMAKE_CURRENT_SOURCE_DIR}/imgui_demo.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/imgui_draw.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/imgui_internal.h
        ${CMAKE_CURRENT_SOURCE_DIR}/imgui_tables.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/imgui_widgets.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/imstb_rectpack.h
        ${CMAKE_CURRENT_SOURCE_DIR}/imstb_textedit.h
        ${CMAKE_CURRENT_SOURCE_DIR}/imstb_truetype.h
        )
target_include_directories(imgui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
