#pragma once

#include <fstream>
#include <iostream>
#include <string>

#include "../sha1/TinySHA1.hpp"

inline int read_torrent_file_content(const std::string& path, std::string& content)
{
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open())
    {
        std::cerr << "Failed to open torrent file" << std::endl;
        return 1;
    }

    content = std::string(
        (std::istreambuf_iterator<char>(file)),
        (std::istreambuf_iterator<char>()));
    return 0;
}

inline std::string compute_hash(const std::string& input)
{
    sha1::SHA1 hasher;
    hasher.processBytes(input.c_str(), input.size());
    uint32_t digest[5];
    hasher.getDigest(digest);
    char tmp[48];
    snprintf(tmp, 45, "%08x%08x%08x%08x%08x", digest[0], digest[1], digest[2], digest[3], digest[4]);
    return tmp;
}

inline std::string encode_info_hash(const std::string& hash) {
    std::string encoded;
    for(auto i = 0; i < hash.size(); i += 2) {
        encoded += '%' + hash.substr(i, 2);
    }
    return encoded;
}

inline std::vector<std::string> get_peaces_hashes(const std::string& pieces_raw)
{
    std::vector<std::string> hashes;

    for(size_t i = 0; i < pieces_raw.size(); i += 20) {
        std::string piece_hash = pieces_raw.substr(i, 20);
        std::stringstream ss;
        // Convert each byte in the slice to hexadecimal format
        for (unsigned char byte : piece_hash){
            ss << std::hex << std::setw(2) << std::setfill('0') << (int)byte;
        }

        hashes.push_back(ss.str());
    }

    return hashes;
}