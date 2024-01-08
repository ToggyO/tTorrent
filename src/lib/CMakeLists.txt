file(GLOB_RECURSE SOURCES LIST_DIRECTORIES true *.h **/*.h)

add_library(${LIB_BINARY} SHARED ${SOURCES})

set_target_properties(${LIB_BINARY} PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(${LIB_BINARY} INTERFACE
    "${CMAKE_CURRENT_SOURCE_DIR}/bencode"
    "${CMAKE_CURRENT_SOURCE_DIR}/nlohmann"
    "${CMAKE_CURRENT_SOURCE_DIR}/json"
    "${CMAKE_CURRENT_SOURCE_DIR}/sha1"
    "${CMAKE_CURRENT_SOURCE_DIR}/torrent"
)
