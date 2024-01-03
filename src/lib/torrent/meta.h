#pragma once

#include <string>

#include "../nlohmann/json.hpp"

using json = nlohmann::json;

constexpr static std::string_view k_announce_key = "announce";
constexpr static std::string_view k_info_key = "info";
constexpr static std::string_view k_info_length_key = "length";

namespace torrent
{
    // TODO: add accessors and setting indicators
    struct Torrent
    {
        std::string announce;
        size_t length;
    };

    Torrent convert_to_torrent_meta(const json& json_object);
}

void __M_process_info_section(torrent::Torrent& meta, const json& info);

torrent::Torrent torrent::convert_to_torrent_meta(const json& json_object)
{
    Torrent meta;

    if (json_object.contains(k_announce_key))
    {
        // Coping data to prevent json object modification
        meta.announce = json_object[k_announce_key];
    }

    if (json_object.contains(k_info_key))
    {
        __M_process_info_section(meta, json_object[k_info_key]);
    }

    // NRVO
    return meta;
}


void __M_process_info_section(torrent::Torrent& meta, const json& info)
{
    if (info.contains(k_info_length_key))
    {
        meta.length = info[k_info_length_key];
    }
}