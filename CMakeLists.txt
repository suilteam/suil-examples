cmake_minimum_required(VERSION 3.8)
project(examples C CXX)

set(SUIL_BASE_PATH "" CACHE STRING "Path to an install of suil package")

# add all example projects
add_subdirectory(basic)
add_subdirectory(websock)
add_subdirectory(process)

# copy for confiuration purposes
file(COPY examples.conf.lua
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})