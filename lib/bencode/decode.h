#pragma once

#include <cctype>
#include <cstdlib>

#include "bencoded_value.h"
#include "../nlohmann/json.hpp"

using namespace bencode;

/** @brief Forward declaration */
inline BEncodedValue __M_decode_bencoded_value(const std::string&, size_t&);

inline Integer __M_decode_integer(const std::string&, size_t&);

inline std::string __M_decode_string(const std::string&, size_t&);

inline BEncodedList __M_decode_list(const std::string&, size_t&);

inline BEncodedDictionary __M_decode_dictionary(const std::string&, size_t&);

namespace bencode
{
    /**
     * @brief Decode string in Bencode format @link https://en.wikipedia.org/wiki/Bencode
     *
     * @param encoded_value string in Bencode format
     *
     * @return result json
     */
    inline BEncodedValue decode(const std::string& encoded_value);
}

BEncodedValue bencode::decode(const std::string& encoded_value)
{
    size_t start_index = 0;
    return __M_decode_bencoded_value(encoded_value, start_index);
}

BEncodedValue __M_decode_bencoded_value(const std::string& encoded_value, size_t& position) {
    if (std::isdigit(encoded_value[position]))
    {
        return __M_decode_string(encoded_value, position);
    }

    if (encoded_value[position] == __k_integer_start_postfix)
    {
        return __M_decode_integer(encoded_value, position);
    }

    if (encoded_value[position] == __k_list_start_postfix)
    {
        return __M_decode_list(encoded_value, position);
    }

    if (encoded_value[position] == __k_dictionary_start_postfix)
    {
        return __M_decode_dictionary(encoded_value, position);
    }

    throw std::runtime_error("Unhandled encoded value: " + encoded_value);
}

Integer __M_decode_integer(const std::string& encoded_value, size_t& position)
{
    // Example: "i52e" -> "52"
    size_t end_index = encoded_value.find( __k_end_postfix, position);
    if (end_index == std::string::npos)
    {
        throw std::runtime_error("Invalid encoded integer: " + encoded_value);
    }

    if (encoded_value[position] == __k_integer_start_postfix) { ++position; }

    auto integer_value = encoded_value.substr(position, end_index - 1);
    position = end_index + 1;

    if (position > encoded_value.size())
    {
        throw std::runtime_error("Unexpected end of data in integer decoding");
    }

    return std::stoll(integer_value);
}

std::string __M_decode_string(const std::string& encoded_value, size_t& position)
{
    // Example: "5:hello" -> "hello"
    size_t colon_index = encoded_value.find(__k_colon, position);
    if (colon_index == std::string::npos)
    {
        throw std::runtime_error("Invalid encoded string: " + encoded_value);
    }

    std::string number_string = encoded_value.substr(position, colon_index);
    int64_t number = std::stoll(number_string);
    std::string str = encoded_value.substr(colon_index + 1, number);

    position = colon_index + 1 + number;
    if (position > encoded_value.size())
    {
        throw std::runtime_error("String length exceeds data size in Bencode");
    }

    return str;
}

BEncodedList __M_decode_list(const std::string& encoded_value, size_t& position)
{
    // Example: "l3:geti52ee" -> ["get",52]
    if (encoded_value[position] == __k_list_start_postfix) { ++position; }

    BEncodedList bEncodedList;
    while (encoded_value[position] != __k_end_postfix)
    {
        bEncodedList.push_back(std::move(__M_decode_bencoded_value(encoded_value, position)));
    }

    if (position >= encoded_value.size())
    {
        throw std::runtime_error("Unexpected end of data in list decoding");
    }

    ++position; // Skip 'e'
    return bEncodedList;
}

BEncodedDictionary __M_decode_dictionary(const std::string& encoded_value, size_t& position)
{
    // Example: "`d3:foo3:bar5:helloi52ee" -> {"foo":"bar","hello":52}
    if (encoded_value[position] == __k_dictionary_start_postfix) { ++position; }

    BEncodedDictionary bEncodedDictionary;
    while (encoded_value[position] != __k_end_postfix)
    {
        std::string key = __M_decode_string(encoded_value, position);
        BEncodedValue value = __M_decode_bencoded_value(encoded_value, position);
        bEncodedDictionary.emplace(std::move(key), std::move(value));
    }

    if (position >= encoded_value.size())
    {
        throw std::runtime_error("Unexpected end of data in dictionary decoding");
    }

    position++;
    return bEncodedDictionary;
}
