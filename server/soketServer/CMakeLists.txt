cmake_minimum_required(VERSION 2.6)
project(socketServer)

set(src_list QueryXML.cpp QueryXML.h  socketServer.cpp)
set(link_flag xml2)

add_executable(socketServer ${src_list})
target_link_libraries(socketServer ${link_flag})
