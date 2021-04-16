cmake_minimum_required(VERSION 3.17)
project(FlappyBird)

find_package(SFML 2 REQUIRED network audio graphics window system)

set(CMAKE_CXX_STANDARD 20)

add_executable(${PROJECT_NAME} main.cpp)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/res DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

include_directories(${SFML_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} sfml-graphics sfml-audio sfml-window sfml-system)