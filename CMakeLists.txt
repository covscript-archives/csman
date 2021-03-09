cmake_minimum_required(VERSION 3.4)
project(csman)

set(CMAKE_CXX_STANDARD 14)

if (MSVC)
    set(CMAKE_CXX_FLAGS "/O2 /EHsc /utf-8 /w")
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
elseif (CMAKE_COMPILER_IS_GNUCXX)
    if (WIN32)
        set(CMAKE_C_FLAGS "--static -fPIC -s -O3")
        set(CMAKE_CXX_FLAGS "--static -fPIC -Og -g")
    else ()
        set(CMAKE_C_FLAGS "-fPIC -s -O3")
        set(CMAKE_CXX_FLAGS "-fPIC -s -O3")
    endif ()
else ()
    set(CMAKE_C_FLAGS "-fPIC -O3")
    set(CMAKE_CXX_FLAGS "-fPIC -O3")
endif ()

include_directories(include)
include_directories(imports/mozart/include)
include_directories(imports/zip/include)

add_subdirectory(imports/mozart)
add_subdirectory(imports/zip)

set(SOURCE_CODE
        sources/global.cpp
        sources/network.cpp
        sources/system.cpp
        sources/main.cpp)

add_executable(csman ${SOURCE_CODE})

target_link_libraries(csman mozart zip curl)

if (WIN32)
    target_compile_definitions(csman PRIVATE CURL_STATICLIB)
    target_link_libraries(csman nghttp2 ssl crypto zstd ssh2 z brotlidec-static brotlicommon-static ws2_32 wldap32 crypt32)
endif ()