#pragma once

#include <string>

#include "../nlohmann/json.hpp"
#include "../bencode/bencoded_value.h"
#include "../json/json_bencode.h"

using namespace bencode;

constexpr static std::string_view k_announce_key = "announce";
constexpr static std::string_view k_info_key = "info";
constexpr static std::string_view k_info_length_key = "length";
constexpr static std::string_view k_info_piece_key = "pieces";
constexpr static std::string_view k_info_piece_length_key = "piece length";

namespace torrent
{
    class Torrent
    {
    public:
        const json raw;
        const std::string announce;
        const Integer length;
        const Integer piece_length;
        const std::string pieces;

        static Torrent from_bencode(const BEncodedValue&);
    };
}

torrent::Torrent torrent::Torrent::from_bencode(const BEncodedValue& value)
{
    json j_raw = value;
    const auto info = j_raw.value(k_info_key, json());
    // NRVO
    return Torrent
    {
        .raw = j_raw,
        .announce = j_raw.value(k_announce_key, "empty"),
        .length = info.value<Integer>(k_info_length_key, 0),
        .piece_length = info.value<Integer>(k_info_piece_length_key, 0),
        .pieces = info.value(k_info_piece_key, ""),
    };
}

// TODO: remove
//namespace torrent
//{
//    struct Torrent
//    {
//        const std::string announce;
//        const Integer length;
//        const Integer piece_length;
//
//    };
//
//    Torrent convert_to_torrent_meta(const BEncodedDictionary&);
//}
//
//void __M_process_info_section(torrent::Torrent& meta, const BEncodedDictionary& info);
//
//torrent::Torrent torrent::convert_to_torrent_meta(const BEncodedDictionary& dictionary)
//{
//    Torrent meta;
//
//    if (auto search = dictionary.find(k_announce_key.data()); search != dictionary.end())
//    {
//        // Coping data to prevent json object modification
//        search->second.try_get_string(meta.announce);
//    }
//
//    if (auto search = dictionary.find(k_info_key.data()); search != dictionary.end())
//    {
//        BEncodedDictionary info;
//        search->second.try_get_dictionary(info);
//        __M_process_info_section(meta, info);
//    }
//
//    // NRVO
//    return meta;
//}
//
//
//void __M_process_info_section(torrent::Torrent& meta, const BEncodedDictionary& info)
//{
//    if (auto search = info.find(k_info_length_key.data()); search != info.end())
//    {
//        search->second.try_get_int(meta.length);
//    }
//}