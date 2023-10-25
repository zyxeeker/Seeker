cmake_minimum_required(VERSION 3.0)

project(mongoose)

set(SRC ${CMAKE_CURRENT_SOURCE_DIR}/mongoose/mongoose.h 
        ${CMAKE_CURRENT_SOURCE_DIR}/mongoose/mongoose.c)

# Prepare library for other executables
add_library(Mongoose STATIC ${SRC})
target_include_directories(Mongoose INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})