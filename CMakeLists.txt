cmake_minimum_required(VERSION 3.4)

project(csman)

set(CMAKE_CXX_STANDARD 14)

include_directories(include)

if (WIN32)
    set(SYSTEM_SRC sources/system/win32.cpp)
ELSEIF(APPLE)
    set(SYSTEM_SRC sources/system/darwin.cpp)
ELSE()
    set(SYSTEM_SRC sources/system/linux.cpp)
ENDIF()

add_executable(csman ${SYSTEM_SRC} sources/csman.cpp)