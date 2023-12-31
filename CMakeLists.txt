cmake_minimum_required(VERSION 3.10)
project(Seeker)

set(CMAKE_CXX_FLAGS -pthread)

set(ENABLE_MONGOOSE ON)

# 3rd dir
set(3RD ${CMAKE_SOURCE_DIR}/3rd)
# nlohmann json
add_subdirectory(${3RD}/nlohmann)
# mongoose(add for test)
add_subdirectory(${3RD}/mongoose)

include_directories(include src)

link_libraries(Nlohmann Mongoose)

add_subdirectory(src)
add_subdirectory(test)

# Copy cfg.json
file(GLOB_RECURSE CFG cfg.json)
file(COPY ${CFG} DESTINATION ./test)