#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "../lib/sha1/sha1.hpp" // TODO: remove ..

// TODO: separate to cpp files
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

// TODO: check or remove
//inline std::string compute_hash(const std::string& input)
//{
//    sha1::SHA1 hasher;
//    hasher.processBytes(input.c_str(), input.size());
//    uint32_t digest[5];
//    hasher.getDigest(digest);
//    char tmp[48];
//    snprintf(tmp, 45, "%08x%08x%08x%08x%08x", digest[0], digest[1], digest[2], digest[3], digest[4]);
//    return tmp;
//}

inline std::string compute_hash(const std::string& input)
{
    SHA1 hasher;
    hasher.update(input);
    return hasher.final();
}

// TODO: check or remove
inline std::string hex_to_string(const std::string& in) {
    std::string output;
    if ((in.length() % 2) != 0) {
        throw std::runtime_error("String is not valid length ...");
    }
    size_t cnt = in.length() / 2;
    for (size_t i = 0; cnt > i; ++i) {
        uint32_t s = 0;
        std::stringstream ss;
        ss << std::hex << in.substr(i * 2, 2);
        ss >> s;
        output.push_back(static_cast<unsigned char>(s));
    }
    return output;
}

inline std::string encode_info_hash(const std::string& hash)
{
    std::string encoded;
    for(auto i = 0; i < hash.size(); i += 2) {
        encoded += '%' + hash.substr(i, 2);
    }
    return encoded;
}

inline std::string to_hex(const std::string_view input)
{
    std::stringstream ss;
    // Convert each byte in the slice to hexadecimal format
    for (unsigned char byte : input){
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)byte;
    }
    return ss.str();
}

inline std::string from_hex(const std::string& input)
{
    char hash[40];
    for (size_t i = 0; i < input.size(); i += 2)
    {
        hash[i / 2] = std::stoi(input.substr(i, 2), nullptr, 16);
    }
    return hash;
}

inline std::vector<std::string> get_peaces_hashes(const std::string& pieces_raw)
{
    std::vector<std::string> hashes;

    for(size_t i = 0; i < pieces_raw.size(); i += 20) {
        std::string piece_hash = pieces_raw.substr(i, 20);
        hashes.push_back(std::move(to_hex(piece_hash)));
    }

    return hashes;
}

inline std::pair<std::string, size_t> split_inet_address(const std::string_view address, char delimiter = ':')
{
    size_t colon_index = address.find(delimiter, 0);
    if (colon_index == std::string::npos)
    {
        throw std::runtime_error("Invalid inet address: " + std::string(address));
    }

    std::string host;
    long long port;
    try
    {
        host = address.substr(0, colon_index);
        port = std::stoll(std::string(address.substr(colon_index + 1)));
    }
    catch (const std::exception& exception)
    {
        const std::string message = "Failed to split inet address: ";
        throw std::runtime_error(message + exception.what());
    }
    return {host, port};
}

template <typename Integer>
Integer bytes_to_integer(const uint8_t* bytes, bool little_endian = true)
{
    Integer result = 0;
    if (little_endian)
    {
        for (size_t i = sizeof(result); i > 0; --i)
        {
            result = (result << 8) | bytes[i];
        }
    }
    else
    {
        for (size_t i = 0; i < sizeof(result); ++i)
        {
            result = (result << 8) | bytes[i];
        }
    }

    return result;
}

// TODO: remove
//template <typename IntegerType>
//IntegerType bitsToInt( IntegerType& result, const unsigned char* bits, bool little_endian = true )
//{
//    result = 0;
//    if (little_endian)
//        for (int n = sizeof( result ); n >= 0; n--)
//            result = (result << 8) +bits[ n ];
//    else
//        for (unsigned n = 0; n < sizeof( result ); n++)
//            result = (result << 8) +bits[ n ];
//    return result;
//}