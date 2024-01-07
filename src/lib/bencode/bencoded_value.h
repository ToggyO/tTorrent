#pragma once

#include <map>
#include <variant>
#include <vector>

struct BEncodedValue;
using BEncodedList = std::vector<BEncodedValue>;
using BEncodedDictionary = std::map<std::string, BEncodedValue>; // unordered_map cannot store incomplete type - BEncodedValue

struct BEncodedValue : std::variant<size_t, std::string, BEncodedList, BEncodedDictionary>
{
    using variant::variant;

    BEncodedValue(const BEncodedValue&) = default;
    BEncodedValue(BEncodedValue&&) noexcept = default;

    BEncodedValue& operator=(const BEncodedValue&) = default;
    BEncodedValue& operator=(BEncodedValue&&) noexcept = default;

    // TODO: remove
//    BEncodedValue(const BEncodedValue& other)
//            : variant(other)
//    {
//        std::cout << "BEncodedValue copy ctor" << std::endl;
//    }
//
//    BEncodedValue(BEncodedValue&& other) noexcept
//            : variant{std::move(other)}
//    {
//        std::cout << "BEncodedValue move ctor" << std::endl;
//    }

    bool try_get_string(std::string& string) const { return try_get_value<std::string>(string); }

    bool try_get_int(size_t& integer) const { return try_get_value<size_t>(integer); }

    bool try_get_list(BEncodedList& list) const { return try_get_value<BEncodedList>(list); }

    bool try_get_dictionary(BEncodedDictionary& string) const { return try_get_value<BEncodedDictionary>(string); }

private:
    template <typename T>
    bool try_get_value(T& result) const
    {
        try
        {
            result = std::get<T>(*this);
            return true;
        }
        catch (const std::bad_variant_access& ex)
        {
            return false;
        }
    }
};

/** @brief Constants */
constexpr static char __k_colon = ':';
constexpr static char __k_end_postfix = 'e';
constexpr static char __k_integer_start_postfix = 'i';
constexpr static char __k_list_start_postfix = 'l';
constexpr static char __k_dictionary_start_postfix = 'd';