cmake_minimum_required(VERSION 3.19)
project(canaletto)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})
find_package( Threads )

configure_file(rsrc/PICT0003.jpg pict001.jpg COPYONLY)
configure_file(rsrc/PICT0004.jpg pict002.jpg COPYONLY)
configure_file(rsrc/PICT0005.jpg pict003.jpg COPYONLY)

find_library(SDL2_IMAGE_LIBRARY
        NAMES SDL2_image
        HINTS ${SDL2_DIR} ${SDL2_DIR}/../../../
        PATH_SUFFIXES lib lib/x64
        )

add_executable(canaletto main.cpp main.h)

target_link_libraries(canaletto ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARY} stdc++fs ${CMAKE_THREAD_LIBS_INIT})