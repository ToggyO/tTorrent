#pragma once

#include <exception>
#include <fstream>
#include <iostream>
#include <string_view>

#include "constants.h"
#include "utils.h"
#include "../lib/bencode/bencoded_value.h" // TODO: remove ..
#include "../lib/bencode/decode.h" // TODO: remove ..
#include "../lib/bencode/encode.h" // TODO: remove ..

using namespace bencode;

// TODO: add descr
class TorrentFile
{
public:
    explicit TorrentFile(std::string_view);

    [[nodiscard]] std::string get_announce() const;

    [[nodiscard]] const std::string& get_info_hash();

    [[nodiscard]] Integer get_file_size() const;

private:
    static void read_torrent_file_content(const std::string&, std::string&);

    BEncodedDictionary get_info() const;

    BEncodedDictionary m_content;

    std::string m_info_hash;
    bool m_info_hash_is_set;
};
