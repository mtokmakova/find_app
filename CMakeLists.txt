cmake_minimum_required(VERSION 3.23)
set(PROJECT_NAME find_app)
project(find_app)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXE_LINKER_FLAGS "-static -static-libgcc -static-libstdc++")

add_executable(find_app main.cpp main.cpp)