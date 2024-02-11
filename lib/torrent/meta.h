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
