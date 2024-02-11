file(GLOB_RECURSE SOURCES LIST_DIRECTORIES true *.h **/*.h)

add_library(${LIB_BINARY} SHARED ${SOURCES})

set_target_properties(${LIB_BINARY} PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(${LIB_BINARY} INTERFACE
    "bencode"
    "json"
    "net"
    "nlohmann"
    "sha1"
    "torrent"
)
