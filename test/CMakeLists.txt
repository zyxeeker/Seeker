set(TEST ${CMAKE_PROJECT_NAME})

add_executable(${TEST} test.cpp)
add_executable(${TEST}_log test_log.cpp)

target_link_libraries(${TEST} ${CMAKE_PROJECT_NAME}_lib)
target_link_libraries(${TEST}_log ${CMAKE_PROJECT_NAME}_lib)