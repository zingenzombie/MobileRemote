cmake_minimum_required(VERSION 3.0)
project(MobileRemote)

set(CMAKE_CXX_STANDARD 17)

add_executable(MobileRemote main.cpp
        RemoteServer.cpp
        RemoteServer.h
        cpp/qrcodegen.cpp
        cpp/qrcodegen.hpp
)


#include_directories("cpp")
#target_link_libraries(MobileRemote "cpp/libqrcodegencpp.a")