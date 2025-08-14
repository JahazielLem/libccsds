cmake_minimum_required(VERSION 3.5)

project(cfdp C)

add_library(cfdp src/cfdp.c)

target_include_directories(cfdp PUBLIC include)