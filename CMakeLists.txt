cmake_minimum_required(VERSION 3.15)
project(art.run LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(art.run art.cpp)

add_subdirectory(minifb)
target_link_libraries(art.run minifb)
