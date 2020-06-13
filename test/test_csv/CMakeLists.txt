add_executable(testcsv testcsv.cpp main.cpp)
target_link_libraries(testcsv gtest_main)
add_test(NAME testcsv COMMAND testcsv)