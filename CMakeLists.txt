cmake_minimum_required(VERSION 3.4)
project(csman)

set(CMAKE_CXX_STANDARD 14)

include_directories(include)
include_directories(imports/mozart/include)
include_directories(imports/zip/include)

add_subdirectory(imports/mozart)
add_subdirectory(imports/zip)

aux_source_directory(sources SRCS)
add_executable(csman ${SRCS})

target_link_libraries(csman mozart zip curl)

if (WIN32)
    target_compile_definitions(csman PRIVATE CURL_STATICLIB)
    target_link_libraries(csman nghttp2 ssl crypto zstd ssh2 z brotlidec-static brotlicommon-static ws2_32 wldap32 crypt32)
endif ()