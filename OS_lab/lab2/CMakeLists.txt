cmake_minimum_required(VERSION 3.29)

project(lab CXX)

add_executable(lab lab.cpp)
# add_library(process STATIC process.cpp)
# target_link_libraries(lab process)

target_compile_features(lab INTERFACE cxx_std_20)
add_compile_options(-ggdb)