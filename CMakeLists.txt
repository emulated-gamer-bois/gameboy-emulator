cmake_minimum_required ( VERSION 3.0.2 )

project ( simple-gl-project )
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# For windows we use our bundled binaries.
if(WIN32)
    list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/external/embree2")
    list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/external/sdl2")
    list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/external/glew")
    list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/external/glm")
endif(WIN32)

macro(config_build_output)
    if(MSVC)
        set(DLL_DIRECTORIES "${CMAKE_SOURCE_DIR}/external/bin")
        set(MSVC_RUNTIME_DIR "${CMAKE_SOURCE_DIR}/bin")
        set_target_properties( ${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY                "${MSVC_RUNTIME_DIR}" )
        set_target_properties( ${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG          "${MSVC_RUNTIME_DIR}" )
        set_target_properties( ${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE        "${MSVC_RUNTIME_DIR}" )
        set_target_properties( ${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${MSVC_RUNTIME_DIR}" )
        set_target_properties( ${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL     "${MSVC_RUNTIME_DIR}" )
        set(vs_user_file "${PROJECT_BINARY_DIR}/${PROJECT_NAME}.vcxproj.user")
        string(REGEX REPLACE "v([0-9][0-9])([0-9])" "\\1.\\2" "VS_TOOLSET_VERSION" "${CMAKE_VS_PLATFORM_TOOLSET}")
        configure_file("${CMAKE_SOURCE_DIR}/VSUserTemplate.user" "${vs_user_file}" @ONLY)
    endif(MSVC)
endmacro(config_build_output)
#Adds /Tests as directory for tests, also adds GTest and Gmock.
include(gtest.cmake)
include(gmock.cmake)

SET(COVERAGE OFF CACHE BOOL "Coverage")
set(CMAKE_CXX_STANDARD 14)

add_executable(tests Tests/Test.cpp)
find_package (Threads)
target_link_libraries(tests PRIVATE gtest ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(tests PRIVATE gmock ${CMAKE_THREAD_LIBS_INIT})
if (MSVC)
    target_compile_options(tests PRIVATE /std:c++17 /W4 /WX)

    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Ox")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Od")

    # VS 2017 removed tr1
    add_definitions(-DGTEST_HAS_TR1_TUPLE=0)
else()
    target_compile_options(tests PRIVATE -std=c++14 -g -Wall -Werror -Wextra -Wpedantic -Wconversion -Wswitch-default -Wswitch-enum -Wunreachable-code -Wwrite-strings -Wcast-align -Wshadow -Wundef)

    if (COVERAGE)
        target_compile_options(tests PRIVATE --coverage)
        target_link_libraries(tests PRIVATE --coverage)
    endif()
endif()

##Ending for tests.
add_definitions(-DGLM_ENABLE_EXPERIMENTAL)
set ( CMAKE_EXPORT_COMPILE_COMMANDS ON )
set ( CMAKE_BUILD_TYPE DEBUG )
add_definitions ( -std=c++11 )
add_subdirectory ( labhelper )
add_subdirectory ( project )
