cmake_minimum_required(VERSION 3.0.0)
SET(PROJECT_VERSION         0)
SET(PROJECT_VERSION_MAJOR   0)
SET(PROJECT_VERSION_MINOR   0)
SET(PROJECT_VERSION_PATCH   0)

project(friasApp DESCRIPTION "menus app for restaurants" LANGUAGES CXX)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_PLATFORM_INDEPENDENT_CODE ON)

add_subdirectory(spdlog)

set(INSTALL_BIN_DIR "/tmp/friasApp")
option(BUILD_CTL "Build drogon_ctl" ON)
option(BUILD_EXAMPLES "Build examples" OFF)
add_subdirectory(drogon)

add_subdirectory(src)

include(CTest)
enable_testing()
add_subdirectory(test)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
