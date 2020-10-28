cmake_minimum_required(VERSION 3.4)
project(csman)

set(CMAKE_CXX_STANDARD 17)

include_directories(mozart/include)
aux_source_directory(mozart/src SRCS_IMPL)
aux_source_directory(mozart/include/mozart++ SRCS)
add_library(mozart STATIC ${SRCS} ${SRCS_IMPL})
link_libraries(mozart)

add_subdirectory(src) #是用了add_lib后才能用add_sub，然后才能直接用这个库吗
add_executable(csman main.cpp)
target_link_libraries(csman csman_src)