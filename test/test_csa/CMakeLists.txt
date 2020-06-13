add_executable(testcsa testcsa.cpp main.cpp)
target_link_libraries(testcsa gtest_main)
add_test(NAME testcsa COMMAND testcsa)