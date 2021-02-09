set(GOOGLEMOCK_ROOT gtest/googlemock CACHE STRING "Google Mock source root")

include_directories(SYSTEM
        ../${GOOGLEMOCK_ROOT}
        ../${GOOGLEMOCK_ROOT}/include
        ${CMAKE_SOURCE_DIR}/${GOOGLEMOCK_ROOT}
        ${CMAKE_SOURCE_DIR}/${GOOGLEMOCK_ROOT}/include
        )

set(GOOGLEMOCK_SOURCES
        ../${GOOGLEMOCK_ROOT}/src/gmock-all.cc
        ../${GOOGLEMOCK_ROOT}/src/gmock_main.cc
        ${CMAKE_SOURCE_DIR}/${GOOGLEMOCK_ROOT}/src/gmock-all.cc
        ${CMAKE_SOURCE_DIR}/${GOOGLEMOCK_ROOT}/src/gmock_main.cc
        )

foreach (_source ${GOOGLEMOCK_SOURCES})
    set_source_files_properties(${_source} PROPERTIES GENERATED 1)
endforeach ()

add_library(gmock ${GOOGLEMOCK_SOURCES})