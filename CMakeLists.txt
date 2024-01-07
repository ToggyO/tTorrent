cmake_minimum_required(VERSION 3.13)

project(bittorrent-starter-cpp)

set(CMAKE_CXX_STANDARD 20) # Enable the C++20 standard

add_subdirectory(tests)

file(GLOB_RECURSE SOURCE_FILES LIST_DIRECTORIES true src/*.h src/*.cpp)

add_executable(bittorrent ${SOURCE_FILES})
set_target_properties(bittorrent PROPERTIES LINKER_LANGUAGE CXX)
