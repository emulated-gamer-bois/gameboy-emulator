
set(GOOGLETEST_ROOT gtest/googletest CACHE STRING "Google Test source root")
message(CMAKE_SOURCE_DIR)
include_directories(SYSTEM
        ../${GOOGLETEST_ROOT}
        ../${GOOGLETEST_ROOT}/include
        ${PROJECT_SOURCE_DIR}/${GOOGLETEST_ROOT}
        ${PROJECT_SOURCE_DIR}/${GOOGLETEST_ROOT}/include
        )

set(GOOGLETEST_SOURCES
        ../${GOOGLETEST_ROOT}/src/gtest-all.cc
        ../${GOOGLETEST_ROOT}/src/gtest_main.cc
        ${CMAKE_SOURCE_DIR}/${GOOGLETEST_ROOT}/src/gtest-all.cc
        ${CMAKE_SOURCE_DIR}/${GOOGLETEST_ROOT}/src/gtest_main.cc
        )

foreach (_source ${GOOGLETEST_SOURCES})
    set_source_files_properties(${_source} PROPERTIES GENERATED 1)
endforeach ()

add_library(gtest ${GOOGLETEST_SOURCES})