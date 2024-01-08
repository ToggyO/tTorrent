#include <gtest/gtest.h>

#include "../src/lib/bencode/encode.h"

TEST(test_bencode_encode, common) {
    std::vector<BEncodedValue> list = { BEncodedValue("kek"), BEncodedValue(69) };

    BEncodedDictionary bEncodedDictionary;
    bEncodedDictionary = {
        { "length", BEncodedValue(234523) }
    };

    BEncodedDictionary data;
    data = {
        { "foo", BEncodedValue("bar") },
        { "hello", BEncodedValue(52) },
        { "list", list },
        { "info", bEncodedDictionary }
    };

    auto value = bencode::encode(data);
    ASSERT_EQ(value, "d3:foo3:bar5:helloi52e4:infod6:lengthi234523ee4:listl3:keki69eee");
}