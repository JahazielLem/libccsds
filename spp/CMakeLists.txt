cmake_minimum_required(VERSION 3.5)

project(spp C)

add_library(spp src/spp.c)

target_include_directories(spp PUBLIC include)