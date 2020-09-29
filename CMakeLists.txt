cmake_minimum_required(VERSION 3.18)
project(csman)

set(CMAKE_CXX_STANDARD 17)

set(jsoncpp_DIR /mnt/d/mywork/cpplib/jsoncpp/include) #your jsoncpp location
set(libjsoncpp_DIR /usr/local/lib/) #your libjsoncpp location
include_directories(jsoncpp_DIR)
link_directories({libjsoncpp_DIR})
link_libraries(jsoncpp)

set(curl_DIR /mnt/d/mywork/cpplib/curl/include) #your curl location
set(libcurl_DIR /usr/local/lib/) #your libcurl location
include_directories(curl_DIR)
link_directories({libcurl_DIR})
link_libraries(curl)

set(Zipper_DIR /mnt/d/mywork/cpplib/zipper/zipper) #your zipper location
set(libZipper_DIR /usr/local/lib/) #your libzipper location
include_directories(Zipper_DIR)
link_directories({libZipper_DIR})
link_libraries(Zipper)

#add_executable(csman main.cpp Command.hpp CsmBase.hpp Config.hpp Config.cpp)
add_executable(csman main.cpp CsmBase.hpp Config.hpp Config.cpp)
target_link_libraries(csman jsoncpp)