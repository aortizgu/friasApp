include_directories(${friasApp_SOURCE_DIR}/spdlog)

add_executable(friasApp main.cpp)
target_link_libraries(friasApp spdlog)