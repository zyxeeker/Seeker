set(TEST ${CMAKE_PROJECT_NAME})

add_executable(${TEST} test.cpp)

target_link_libraries(${TEST} ${CMAKE_PROJECT_NAME}_lib)