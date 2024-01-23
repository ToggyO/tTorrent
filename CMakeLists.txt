cmake_minimum_required(VERSION 3.13)

# Prevents error "The DOWNLOAD_EXTRACT_TIMESTAMP option was not given and policy CMP0135 is"
if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
    set(CMAKE_POLICY_DEFAULT_CMP0135 NEW)
endif()

project(bittorrent-starter-cpp)

set(CMAKE_CXX_STANDARD 20) # Enable the C++20 standard

add_subdirectory(tests)

file(GLOB_RECURSE SOURCE_FILES LIST_DIRECTORIES true src/*.h src/*.cpp)

add_executable(bittorrent ${SOURCE_FILES}
        src/cmd_processing/process_handshake.cpp
        src/lib/sha1/sha1.hpp)

set_target_properties(bittorrent PROPERTIES LINKER_LANGUAGE CXX)
