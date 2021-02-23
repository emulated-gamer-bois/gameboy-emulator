cmake_minimum_required ( VERSION 3.0.2 )

project ( lame-boy )
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# For windows we use our bundled binaries.
if(WIN32)
    list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/external/sdl2")
    list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/external/glew")
    list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/external/glm")
endif(WIN32)

macro(config_build_output)
    if(MSVC)
        set_target_properties( ${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY                "${RUNTIME_DIR}" )
        set_target_properties( ${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG          "${RUNTIME_DIR}" )
        set_target_properties( ${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE        "${RUNTIME_DIR}" )
        set_target_properties( ${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${RUNTIME_DIR}" )
        set_target_properties( ${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL     "${RUNTIME_DIR}" )
        set(vs_user_file "${PROJECT_BINARY_DIR}/${PROJECT_NAME}.vcxproj.user")
        string(REGEX REPLACE "v([0-9][0-9])([0-9])" "\\1.\\2" "VS_TOOLSET_VERSION" "${CMAKE_VS_PLATFORM_TOOLSET}")
    endif(MSVC)
endmacro(config_build_output)


if(WIN32 AND ("${CMAKE_GENERATOR}" MATCHES "NMake.*"))
    set(DLL_DIRECTORIES "${CMAKE_SOURCE_DIR}/external/bin")
    set(RUNTIME_DIR "${CMAKE_SOURCE_DIR}/bin")
    configure_file("${CMAKE_SOURCE_DIR}/.run/project.run.xml.in" "${CMAKE_SOURCE_DIR}/.run/project.run.xml" @ONLY)
endif()

add_definitions(-DGLM_ENABLE_EXPERIMENTAL)
set ( CMAKE_EXPORT_COMPILE_COMMANDS ON )
set ( CMAKE_BUILD_TYPE DEBUG )
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
endif()

# Excludes graphics code if tests are run in travis
if(NOT TRAVIS)
    add_subdirectory ( labhelper )
endif()

add_subdirectory ( src )

add_subdirectory(tests)
