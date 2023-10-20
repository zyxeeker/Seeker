cmake_minimum_required(VERSION 3.0)

project(nlohmann)
    
# Prepare library for other executables
add_library(Nlohmann INTERFACE)
target_include_directories(Nlohmann INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})