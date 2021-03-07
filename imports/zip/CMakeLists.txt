cmake_minimum_required(VERSION 3.4)
project(libz)
set(CMAKE_CXX_STANDARD 14)

include_directories(include)
add_library(zip zip.c)