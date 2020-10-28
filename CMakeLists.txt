cmake_minimum_required(VERSION 3.4)
project(csman)

set(CMAKE_CXX_STANDARD 14)

include_directories(include)

add_subdirectory(imports/mozart)
add_subdirectory(imports/zipper)
add_subdirectory(imports/jsoncpp)

include_directories(imports/mozart/include)
include_directories(imports/jsoncpp/include)

aux_source_directory(src SRCS)

add_executable(csman ${SRCS})

target_link_libraries(csman mozart staticZipper jsoncpp_lib curl)