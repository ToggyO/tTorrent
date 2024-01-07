find_package(GTest REQUIRED)

enable_testing()

file(GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES true *.h test_*.cpp)
add_executable(tests ${TEST_SOURCES}
        test_bencode_encode.cpp)

target_include_directories(tests
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/lib/bencode
)
target_link_libraries(tests PUBLIC
    GTest::gtest
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(tests)

target_include_directories(tests PRIVATE ${CMAKE_CURRENT_BINARY_DIR})