#pragma once

#include <vector>
#include <cctype>
#include <cstdlib>

#include "../nlohmann/json.hpp"

using json = nlohmann::json;

/** @brief Constants */
constexpr static char end_postfix = 'e';
constexpr static char integer_start_postfix = 'i';
constexpr static char list_start_postfix = 'l';

/** @brief Forward declaration */
json __M_decode_bencoded_value(const std::string&, size_t&);

json __M_decode_integer(const std::string&, size_t&);

json __M_decode_string(const std::string&, size_t&);

json __M_decode_list(const std::string&, size_t&);

namespace bencode
{
    /**
     * @brief Decode string in Bencode format @link https://en.wikipedia.org/wiki/Bencode
     *
     * @param encoded_value string in Bencode foramt
     *
     * @return result json
     */
    json decode(const std::string& encoded_value);
}

json bencode::decode(const std::string& encoded_value)
{
    size_t start_index = 0;
    return __M_decode_bencoded_value(encoded_value, start_index);
}

json __M_decode_bencoded_value(const std::string& encoded_value, size_t& position) {
    if (std::isdigit(encoded_value[position]))
    {
        return __M_decode_string(encoded_value, position);
    }

    if (encoded_value[position] == integer_start_postfix)
    {
        return __M_decode_integer(encoded_value, position);
    }

    if (encoded_value[position] == list_start_postfix)
    {
        return __M_decode_list(encoded_value, position);
    }

    throw std::runtime_error("Unhandled encoded value: " + encoded_value);
}

json __M_decode_integer(const std::string& encoded_value, size_t& position)
{
    // Example: "i52e" -> "52"
    size_t end_index = encoded_value.find( end_postfix, position);
    if (end_index == std::string::npos)
    {
        throw std::runtime_error("Invalid encoded integer: " + encoded_value);
    }

    if (encoded_value[position] == integer_start_postfix) { ++position; }

    auto integer_value = encoded_value.substr(position, end_index - 1);
    position = end_index + 1;
    return std::atoll(integer_value.c_str());
}

json __M_decode_string(const std::string& encoded_value, size_t& position)
{
    // Example: "5:hello" -> "hello"
    size_t colon_index = encoded_value.find(':', position);
    if (colon_index == std::string::npos)
    {
        throw std::runtime_error("Invalid encoded string: " + encoded_value);
    }

    std::string number_string = encoded_value.substr(position, colon_index);
    int64_t number = std::atoll(number_string.c_str());
    std::string str = encoded_value.substr(colon_index + 1, number);

    position = colon_index + 1 + number;
    return str;
}

json __M_decode_list(const std::string& encoded_value, size_t& position)
{
    // Example: "l3:geti52ee" -> ["get",52]
    if (encoded_value[position] == list_start_postfix) { ++position; }

    auto json_array = json::array();
    while (encoded_value[position] != end_postfix)
    {
        json_array.push_back(__M_decode_bencoded_value(encoded_value, position));
    }

    ++position; // Skip 'e'
    return json_array;
}