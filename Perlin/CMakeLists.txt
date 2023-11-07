cmake_minimum_required(VERSION 3.26)
project(Perlin)

set(CMAKE_CXX_STANDARD 23)

add_library(Perlin INTERFACE
        perlin.hpp
        internal.hpp
        simplex.hpp)

add_executable(PerlinDemo demo.cpp)
