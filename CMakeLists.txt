cmake_minimum_required(VERSION 3.18)
project(csman)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)
aux_source_directory(src SRCS_IMPL)
aux_source_directory(include/mozart++ SRCS)
add_library(mozart STATIC ${SRCS} ${SRCS_IMPL})
link_libraries(mozart)

set(jsoncpp_DIR /mnt/d/mywork/cpplib/jsoncpp/include)
set(libjsoncpp_DIR /usr/local/lib/)
include_directories(jsoncpp_DIR)
link_directories({libjsoncpp_DIR})
link_libraries(jsoncpp)

set(curl_DIR /mnt/d/mywork/cpplib/curl/include)
set(libcurl_DIR /usr/local/lib/)
include_directories(curl_DIR)
link_directories({libcurl_DIR})
link_libraries(curl)

set(Zipper_DIR /mnt/d/mywork/cpplib/zipper/zipper)
set(libZipper_DIR /usr/local/lib/)
include_directories(Zipper_DIR)
link_directories({libZipper_DIR})
link_libraries(Zipper)


#add_executable(csman main.cpp CsmBase.hpp Config.hpp Config.cpp)
add_executable(csman main.cpp CsmBase.hpp CsmBase.cpp)
target_link_libraries(csman jsoncpp)