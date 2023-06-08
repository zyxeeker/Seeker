cmake_minimum_required(VERSION 3.10)
project(Seeker)

include_directories(include src 3rd)

add_subdirectory(src)
add_subdirectory(test)

# Copy cfg.json
file(GLOB_RECURSE CFG cfg.json)
file(COPY ${CFG} DESTINATION ./test)