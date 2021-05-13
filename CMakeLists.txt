cmake_minimum_required ( VERSION 3.0.2 )

project ( lame-boy )
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set ( CMAKE_CXX_STANDARD            17)
set ( CMAKE_CXX_STANDARD_REQUIRED   ON)
set ( CMAKE_EXPORT_COMPILE_COMMANDS ON )
set ( CMAKE_BUILD_TYPE              DEBUG )

#Makes the emulator go swoosh
if(MSVC)
    set ( CMAKE_CXX_FLAGS_DEBUG   "/O2" )
    set ( CMAKE_CXX_FLAGS_RELEASE "/O2" )
else()
    set ( CMAKE_CXX_FLAGS_DEBUG   "-O3" )
    set ( CMAKE_CXX_FLAGS_RELEASE "-O3" )
endif()

add_definitions( -DGLM_ENABLE_EXPERIMENTAL )
if( CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang" )
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17" )
endif()

# All libraries made is static
if (MSVC)
    string(REPLACE "/MD" /MT CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE})
    string(REPLACE "/MDd" /MTd CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})
endif ()

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

add_subdirectory ( src )
add_subdirectory ( tests )