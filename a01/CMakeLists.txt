cmake_minimum_required(VERSION 3.13)
project(a01)

set(CMAKE_CXX_STANDARD 14)

add_executable(a01 tokenizer.cpp)
target_link_libraries(a01 readline)