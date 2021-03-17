cmake_minimum_required(VERSION 3.4)
project(covscript_system)
set(CMAKE_CXX_STANDARD 14)

include_directories(include)
add_library(cs_system src/common.cpp)