cmake_minimum_required(VERSION 3.12)
project(PeculiarCompiler LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 添加子目录（src）
add_subdirectory(src)
