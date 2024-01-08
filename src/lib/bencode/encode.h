#pragma once

#include <string>
#include <sstream>
#include <variant>

#include "bencoded_value.h"

using namespace bencode;

/** @brief Forward declaration */
std::string __M_encode(const BEncodedValue&);

struct __encoder;

namespace bencode
{
    // TODO: add descr
    std::string encode(const BEncodedValue& value) { return __M_encode(value); }
}

struct __encoder
{
    [[nodiscard]] std::string operator()(const std::string& str)
    {
        std::stringstream out;
        out << str.size() << __k_colon << str;
        return out.str();
    }

    [[nodiscard]] std::string operator()(const size_t& num)
    {
        std::stringstream out;
        out << __k_integer_start_postfix << num << __k_end_postfix;
        return out.str();
    }

    [[nodiscard]] std::string operator()(const BEncodedList& list)
    {
        std::stringstream out;
        out << __k_list_start_postfix;

        for (const auto& item : list)
        {
            out << __M_encode(item);
        }

        out << __k_end_postfix;
        return out.str();
    }

    [[nodiscard]] std::string operator()(const BEncodedDictionary& dict)
    {
        std::stringstream out;
        out << __k_dictionary_start_postfix;

        for (const auto& pair : dict)
        {
            auto key = __M_encode(pair.first);
            auto value = __M_encode(pair.second);
            out << key << value;
        }

        out << __k_end_postfix;
        return out.str();
    }
};

std::string __M_encode(const BEncodedValue& value) { return std::visit(__encoder{}, value); }