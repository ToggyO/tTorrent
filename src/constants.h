#pragma once

#include <string>
#include <string_view>

struct Constants
{
    static inline int const peer_ip_byte_size = 6;
    static inline int const client_port = 6881;
    static inline char const question_char = '?';
    static inline char const ampersand_char = '&';
    static inline char const equality_sign_char = '=';
    static inline char const point_char = '.';

    static inline std::string const announce = "announce";
    static inline std::string const info = "info";
    static inline std::string const info_hash = "info_hash";
    static inline std::string const file_size = "length";
    static inline std::string const peer_id = "peer_id";
    static inline std::string const port = "port";
    static inline std::string const uploaded = "uploaded";
    static inline std::string const downloaded = "downloaded";
    static inline std::string const left = "left";
    static inline std::string const compact = "compact";
    static inline std::string const protocol = "BitTorrent protocol";

};

