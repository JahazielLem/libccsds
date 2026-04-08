add_executable(example_spp
    example_spp.c
)

target_link_libraries(example_spp
    PRIVATE ccsds
)