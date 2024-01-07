#pragma once

#include <fstream>
#include <iostream>
#include <string>

#include "../sha1/TinySHA1.hpp"

int read_torrent_file_content(const std::string& path, std::string& content)
{
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open())
    {
        std::cerr << "Failed to open torrent file";
        return 1;
    }

    content = std::string(
            (std::istreambuf_iterator<char>(file)),
            (std::istreambuf_iterator<char>()));
    return 0;
}

std::string compute_hash(const std::string& input)
{
    sha1::SHA1 hasher;
    hasher.processBytes(input.c_str(), input.size());
    uint32_t digest[5];
    hasher.getDigest(digest);
    char tmp[48];
    snprintf(tmp, 45, "%08x%08x%08x%08x%08x", digest[0], digest[1], digest[2], digest[3], digest[4]);
    return tmp;
}