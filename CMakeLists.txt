cmake_minimum_required(VERSION 3.3)
project(ProxyServer)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(TARGET_NAME "ProxyServer")

set(SOURCE src/main.cpp src/ClientHandler.cpp src/ProxyServer.cpp src/Utils.cpp)
add_executable(${TARGET_NAME} ${SOURCE})