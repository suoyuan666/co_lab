cmake_minimum_required(VERSION 3.10)

project(lab CXX)

add_executable(lab lab.cpp)

target_compile_features(lab INTERFACE cxx_std_20)
add_compile_options(-g)