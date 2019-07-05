cmake_minimum_required(VERSION 3.13)
project(a03_pagetable)

set(CMAKE_CXX_STANDARD 14)

add_executable(a03_pagetable main.cpp level.cpp PageTable.cpp byu_tracereader.c)