cmake_minimum_required(VERSION 3.16)

execute_process(
    COMMAND git describe --tags --dirty --always
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

string(REGEX REPLACE "^v" "" GIT_VERSION ${GIT_VERSION})

string(REGEX MATCH "^[0-9]+\\.[0-9]+\\.[0-9]+" PROJECT_VERSION_CLEAN ${GIT_VERSION})

if (NOT PROJECT_VERSION_CLEAN)
    set(PROJECT_VERSION_CLEAN "0.0.0")
endif()

project(ccsds
    VERSION ${PROJECT_VERSION}
    DESCRIPTION "CCSDS Space Packet Protocol (SPP) Library"
    LANGUAGES C
)

configure_file(
    ${CMAKE_SOURCE_DIR}/include/version.h.in
    ${CMAKE_BINARY_DIR}/generated/version.h
    @ONLY
)

option(CCSDS_BUILD_TESTS "Build tests" ON)
option(CCSDS_BUILD_EXAMPLES "Build examples" ON)
option(CCSDS_INSTALL "Enable installation" ON)

add_library(ccsds
    src/spp/spp.c
)

add_library(ccsds::ccsds ALIAS ccsds)

target_include_directories(ccsds
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/generated>
        $<INSTALL_INTERFACE:include>
)

target_compile_features(ccsds PUBLIC c_std_99)

if (CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(ccsds PRIVATE -Wall -Wextra -Werror -Wpedantic)
elseif (MSVC)
    target_compile_options(ccsds PRIVATE /W4)
endif()

if (CCSDS_BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()

if (CCSDS_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if (CCSDS_INSTALL)

    include(GNUInstallDirs)

    install(TARGETS ccsds
            EXPORT ccsdsTargets
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )

    install(DIRECTORY include/
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )

    install(EXPORT ccsdsTargets
            FILE ccsdsConfig.cmake
            NAMESPACE ccsds::
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ccsds
    )

endif()