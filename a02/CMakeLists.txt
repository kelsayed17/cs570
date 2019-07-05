cmake_minimum_required(VERSION 3.13)
project(a02)

set(CMAKE_CXX_STANDARD 14)

add_executable(a02 main.cpp command.cpp tokenizer.cpp)
target_link_libraries(a02 readline)