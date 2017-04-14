cmake_minimum_required(VERSION 2.8)
project(SimSearcher)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp SimSearcher.cpp)
add_executable(SimSearcher ${SOURCE_FILES})