cmake_minimum_required(VERSION 3.14.5)
project (one_integration)

add_definitions(-DVERSION="0.0.1")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory (src) 
