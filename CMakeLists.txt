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

aux_source_directory(include/csman CSMAN_INCLUDE)
aux_source_directory(include/csman/components CSMAN_COMPONENTS)

add_executable(csman ${CSMAN_COMPONENTS} ${CSMAN_INCLUDE} ${SYSTEM_SRC} sources/csman.cpp)