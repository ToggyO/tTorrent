#pragma once

#include <string>

#include "../nlohmann/json.hpp"

using json = nlohmann::json;

constexpr static std::string_view k_announce_key = "announce";
constexpr static std::string_view k_info_key = "info";
constexpr static std::string_view k_info_length_key = "length";

namespace torrent
{
    struct Torrent
    {
        std::string announce;
        size_t length;
    };

    Torrent convert_to_torrent_meta(const BEncodedDictionary&);
}

void __M_process_info_section(torrent::Torrent& meta, const BEncodedDictionary& info);

torrent::Torrent torrent::convert_to_torrent_meta(const BEncodedDictionary& dictionary)
{
    Torrent meta;

    if (auto search = dictionary.find(k_announce_key.data()); search != dictionary.end())
    {
        // Coping data to prevent json object modification
        search->second.try_get_string(meta.announce);
    }

    if (auto search = dictionary.find(k_info_key.data()); search != dictionary.end())
    {
        BEncodedDictionary info;
        search->second.try_get_dictionary(info);
        __M_process_info_section(meta, info);
    }

    // NRVO
    return meta;
}


void __M_process_info_section(torrent::Torrent& meta, const BEncodedDictionary& info)
{
    if (auto search = info.find(k_info_length_key.data()); search != info.end())
    {
        search->second.try_get_int(meta.length);
    }
}