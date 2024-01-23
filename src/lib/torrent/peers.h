#pragma once

#include <iostream>
#include <string>
#include <sstream>
#if defined(_WIN32) || defined(_WIN64)
#include <winsock2.h>
#endif
#if defined(linux) || defined(__linux__) || defined(__linux) || defined(__APPLE__) && defined(__MACH__)
#include <sys/socket.h>
#include <arpa/inet.h>
#endif

#include "../http/HTTPRequest.hpp"
#include "../bencode/bencoded_value.h"
#include "../bencode/decode.h"

constexpr static char __k_question_char = '?';
constexpr static char __k_ampersand_char = '&';
constexpr static char __k_equality_sign_char = '=';
constexpr static char __k_point_char = '.';
constexpr static std::string_view __k_info_hash = "info_hash";
constexpr static std::string_view __k_peer_id = "peer_id";
constexpr static std::string_view __k_port = "port";
constexpr static std::string_view __k_uploaded = "uploaded";
constexpr static std::string_view __k_downloaded = "downloaded";
constexpr static std::string_view __k_left = "left";
constexpr static std::string_view __k_compact = "compact";
constexpr static int __k_peer_ip_byte_size = 6;
constexpr static std::string_view __k_protocol = "BitTorrent protocol";

// TODO: add descr
// TODO: win implementation
namespace torrent
{
    using namespace bencode;

    struct TorrentDownloadInfo
    {
        std::string info_hash;
        std::string peer_id;
        size_t port;
        size_t uploaded;
        size_t downloaded;
        size_t left;
        ushort compact;
    };

    inline std::string get_peers_info(std::string&& announce, TorrentDownloadInfo&& info)
    {
        std::stringstream ss;

        ss << announce << __k_question_char
            << __k_info_hash << __k_equality_sign_char << info.info_hash << __k_ampersand_char
            << __k_peer_id << __k_equality_sign_char << info.peer_id << __k_ampersand_char
            << __k_port << __k_equality_sign_char << info.port << __k_ampersand_char
            << __k_uploaded << __k_equality_sign_char << info.uploaded << __k_ampersand_char
            << __k_downloaded << __k_equality_sign_char << info.downloaded << __k_ampersand_char
            << __k_left << __k_equality_sign_char << info.left << __k_ampersand_char
            << __k_compact << __k_equality_sign_char << info.compact;

        auto k = ss.str();
        http::Request request{k};

        auto response = request.send("GET");
        return std::string{response.body.begin(), response.body.end()};
    }

    inline bool try_extract_peers(const std::string& bencoded_peers, std::vector<std::string>& result)
    {
        auto decoded_peers_info = decode(bencoded_peers);
        const auto decoded_peers_info_dict = reinterpret_cast<BEncodedDictionary*>(&decoded_peers_info);

        auto search = decoded_peers_info_dict->find("peers");
        if (search == decoded_peers_info_dict->end())
        {
            std::cerr << "Missing peers in received value: \"" << bencoded_peers << "\"" << std::endl;
            return false;
        }

        std::string peers;
        search->second.try_get_string(peers);

        std::stringstream ss;
        for (int i = 0; i < peers.size(); i += __k_peer_ip_byte_size)
        {
            ss.clear();
            ss.str("");

            ushort port = ((unsigned short)peers[i + 4] << 8) | (unsigned char)peers[i + 5];
            ss
                << std::to_string((unsigned char)peers[i]) << __k_point_char
                << std::to_string((unsigned char)peers[i + 1]) << __k_point_char
                << std::to_string((unsigned char)peers[i + 2]) << __k_point_char
                << std::to_string((unsigned char)peers[i + 3])
                << __k_colon << port;

            result.push_back(ss.str());
        }

        return true;
    }

    // TODO: win implementation
    inline int make_peer_handshake(const std::string& domain, size_t port, const std::string& info_hash, std::string& result)
    {
        int sock;
#if defined(_WIN32) || defined(_WIN64)
        sock = WSAStartup(MAKEWORD(2,2), &wsaData);
#endif
#if defined(linux) || defined(__linux__) || defined(__linux) || defined(__APPLE__) && defined(__MACH__)
        sock = socket(AF_INET, SOCK_STREAM, 0);
#endif
        if (sock == -1)
        {
            std::cerr << "Error creating socket" << std::endl;
            return 1;
        }

        sockaddr_in hint{};
        hint.sin_family = AF_INET;
        hint.sin_port = htons(port);
        if (inet_pton(AF_INET, domain.c_str(), &hint.sin_addr) <= 0)
        {
            std::cerr << "Error converting IP address" << std::endl;
            close(sock);
            return 1;
        }

        int connect_result = connect(sock, (struct sockaddr*)&hint, sizeof(hint));
        if (connect_result == -1)
        {
            std::cerr << "Error connecting to the server: " << errno << std::endl;
            close(sock);
            return 1;
        }

//        unsigned char length = 19;
////        unsigned char protocol[20] = "BitTorrent protocol";
//        unsigned char bytes_to_skip[8] = {0};

        char hash[40];
        for (size_t i = 0; i < info_hash.size(); i += 2)
            hash[i / 2] = std::stoi(info_hash.substr(i, 2), nullptr, 16);

//        std::string hash = hex_to_string(info_hash);
//        std::string self_id = "00112233445566778899";

        const size_t len = 69;
        char msg[len];
        msg[0] = 19;
        std::strcpy(msg + 1, "BitTorrent protocol");
        std::memset(msg + 20, 0, 8);
        std::strcpy(msg + 28, hash);
        std::strcpy(msg + 48, "00112233445566778899");
//        std::stringstream msg;
//        msg
//                << length
//                << __k_protocol
//                << bytes_to_skip
//                << hash
//                << self_id; // TODO: generate peer id

//        msg
//            << 19
//            << __k_protocol
//            << bytes_to_skip
//            << info_hash
//            << "00112233445566778899"; // TODO: generate peer id

//        auto message = msg.str();
//        auto message = std::string(msg);
//        size_t transfer_msg_size = message.size() + 1; // TODO: проверить + 1
//        auto response = send(sock, message.c_str(), transfer_msg_size, 0); //  message.size() + 1 includes Clang null terminator
        auto response = send(sock, msg, len - 1, 0); //  message.size() + 1 includes Clang null terminator
        if (response == -1)
        {
            std::cerr << "Error sending data" << std::endl;
            return 1;
        }

        char buffer[1024] = {0};
//        memset(&buffer, 0, transfer_msg_size);

        ssize_t received = recv(sock, buffer, sizeof(buffer), 0);
        if (received < 0)
        {
            std::cerr << "Error receiving data" << std::endl;
            return 1;
        }

        close(sock);
//        char buffer[] = "3�Hp\u007F�Ӣp�|�f�\u007Fm�g���R�Nn鋫g|-�_2Y9�-q\u000Eo����\u000E\u0016]�\u001D\u001F�c��Z{��i�o)��E��U�����u�]e\u007F����\u000E �\u0005\u0005�� �e\u0005��(�g�'6�\u001B\u0013\u000E_,�O";


//        std::string recv_peer_id(buffer + 48, buffer + 68);
//        std::string recv_peer_id = std::string(buffer, buffer + (len - 1));
//        std::stringstream ss;
//        for (unsigned char c : recv_peer_id) {
//            ss << std::hex << std::setfill('0') << std::setw(2)
//               << static_cast<int>(c);
//        }
//        result = ss.str();
//        result = std::string(buffer, received); TODO: remove
        result = std::string(buffer, buffer + received);

        return 0;
    }
}