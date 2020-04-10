cmake_minimum_required(VERSION 3.10)

project(RawImageViewer)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB SOURCES "*.cpp")
 
add_executable(RawImageViewer ${SOURCES})

target_link_libraries(RawImageViewer SDL2 pthread)
