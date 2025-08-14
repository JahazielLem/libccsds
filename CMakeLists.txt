cmake_minimum_required(VERSION 3.10)
project(ccsds LANGUAGES C)

# Versión
set(PROJECT_MAJOR_VERSION 0)
set(PROJECT_MINOR_VERSION 0)
set(PROJECT_PATCH_VERSION 1)

include_directories(
        ${CMAKE_SOURCE_DIR}/cfdp
        ${CMAKE_SOURCE_DIR}/cfdp/src/
        ${CMAKE_SOURCE_DIR}/cfdp/include
        ${CMAKE_SOURCE_DIR}/spp
        ${CMAKE_SOURCE_DIR}/spp/src/
        ${CMAKE_SOURCE_DIR}/spp/include
        ${CMAKE_SOURCE_DIR}/main/
)
add_definitions(${GTK_CFLAGS_OTHER})

file(GLOB MAIN_SOURCES
        "cfdp/src/*.c"
        "spp/src/*.c"
        "main/main.c"
)

add_executable(ccsds ${MAIN_SOURCES})

# Output
set_target_properties(ccsds PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_INSTALL_RPATH "${CMAKE_BINARY_DIR}")
set_target_properties(ccsds PROPERTIES
        BUILD_WITH_INSTALL_RPATH TRUE
        INSTALL_RPATH "${CMAKE_BINARY_DIR}"
)
