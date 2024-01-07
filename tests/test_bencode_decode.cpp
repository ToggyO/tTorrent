#include <gtest/gtest.h>

#include "../src/lib/bencode/decode.h"

TEST(test_bencode_decode, string_value) {
    auto encoded_value = "10:created by";

    auto value = bencode::decode(encoded_value);

    std::string result;
    ASSERT_TRUE(value.try_get_string(result));
    ASSERT_EQ("created by", result);
}

TEST(test_bencode_decode, integer_value) {
    auto encoded_value = "i952e";

    auto value = bencode::decode(encoded_value);

    size_t result;
    ASSERT_TRUE(value.try_get_int(result));
    ASSERT_EQ(952, result);
}

TEST(test_bencode_decode, list_value) {
    auto encoded_value = "l3:keki52ee";

    auto value = bencode::decode(encoded_value);

    BEncodedList result;
    ASSERT_TRUE(value.try_get_list(result));

    std::string str;
    ASSERT_TRUE(result[0].try_get_string(str));
    ASSERT_EQ(str, "kek");

    size_t integer;
    ASSERT_TRUE(result[1].try_get_int(integer));
    ASSERT_EQ(integer, 52);
}

TEST(test_bencode_decode, dictionary_value) {
    auto encoded_value = "d3:foo3:bar5:helloi52ee";

    auto value = bencode::decode(encoded_value);

    BEncodedDictionary result;
    ASSERT_TRUE(value.try_get_dictionary(result));

    auto search = result.find("foo");
    ASSERT_TRUE(search != result.end());
    std::string val1;
    ASSERT_TRUE(search->second.try_get_string(val1));
    ASSERT_EQ(val1, "bar");

    search = result.find("hello");
    ASSERT_TRUE(search != result.end());
    size_t val2;
    ASSERT_TRUE(search->second.try_get_int(val2));
    ASSERT_EQ(val2, 52);
}