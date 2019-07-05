cmake_minimum_required(VERSION 3.13)
project(a03_threads)

set(CMAKE_CXX_STANDARD 14)

add_executable(a03_threads thread_demo.cpp worker.cpp)