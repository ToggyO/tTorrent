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

    std::string get_peers_info(std::string&& announce, TorrentDownloadInfo&& info)
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

    bool try_extract_peers(const std::string& bencoded_peers, std::vector<std::string>& result)
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
    inline int make_peer_handshake(const std::string& domain, size_t port, const std::string_view info_hash, std::string& result)
    {
        int sock = socket(AF_INET, SOCK_STREAM, 0);
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

        int connect_result = connect(sock, (sockaddr*)&hint, sizeof(hint));
        if (connect_result == -1)
        {
            std::cerr << "Error connecting to the server" << std::endl;
            close(sock);
            return 1;
        }

        std::stringstream msg;
        msg
            << 19
            << __k_protocol
            << (unsigned char)0 << (unsigned char)0 << (unsigned char)0 << (unsigned char)0 << (unsigned char)0 << (unsigned char)0 << (unsigned char)0 << (unsigned char)0 // TODO: refactor this shieeeeeeeeeet
            << info_hash
            << "00112233445566778899"; // TODO: generate peer id

        auto message = msg.str();
        size_t transfer_msg_size = message.size() + 1; // TODO: проверить + 1
        auto response = send(sock, message.c_str(), transfer_msg_size, 0); //  message.size() + 1 includes Clang null terminator
        if (response == -1)
        {
            std::cerr << "Error sending data" << std::endl;
            return 1;
        }

        char buffer[transfer_msg_size];
        memset(&buffer, 0, transfer_msg_size);

        ssize_t received = 0;
        do
        {
            received += recv(sock, buffer, transfer_msg_size, 0);
        } while (received < transfer_msg_size);

        close(sock);

        result = std::string(buffer, received);

        return 0;
    }
}