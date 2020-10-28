cmake_minimum_required(VERSION 3.4)
project(csman)

set(CMAKE_CXX_STANDARD 14)

include_directories(include)

aux_source_directory(src SRCS)

add_library(mozart $ {SRCS})