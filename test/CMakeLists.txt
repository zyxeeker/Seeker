set(TEST ${CMAKE_PROJECT_NAME})

add_executable(${TEST}_log test_log.cpp)
add_executable(${TEST}_cfg test_cfg.cpp)
add_executable(${TEST}_thread test_thread.cpp)

target_link_libraries(${TEST}_log ${CMAKE_PROJECT_NAME}_lib)
target_link_libraries(${TEST}_cfg ${CMAKE_PROJECT_NAME}_lib)
target_link_libraries(${TEST}_thread ${CMAKE_PROJECT_NAME}_lib)

# Copy test.json for test
file(GLOB_RECURSE CFG test.json)
file(COPY ${CFG} DESTINATION .)