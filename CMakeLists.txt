cmake_minimum_required(VERSION 3.10)

project(RawImageViewer)

file(GLOB SOURCES "*.cpp")
 
add_executable(RawImageViewer ${SOURCES})

target_link_libraries(RawImageViewer SDL2 pthread)
