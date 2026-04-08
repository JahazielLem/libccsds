function(add_ccsds_test name source)
    add_executable(${name} ${source})
    target_link_libraries(${name} PRIVATE ccsds)
    add_test(NAME ${name} COMMAND ${name})
endfunction()

add_ccsds_test(test_spp_build test_spp_build.c)
add_ccsds_test(test_spp_unpack test_spp_unpack.c)
add_ccsds_test(test_spp_invalid test_spp_invalid.c)