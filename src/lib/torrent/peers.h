#pragma once

#include <string>
#include <sstream>

#include "../http/HTTPRequest.hpp"

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

// TODO: add descr
namespace torrent
{
    struct TorrentDownloadInfo
    {
        std::string info_hash;
        std::string peer_id;
        size_t port;
        size_t uploaded;
        size_t downloaded;
        size_t left;
        ushort compact;
// TODO: remove?
//        TorrentDownloadInfo(
//                std::string&& info_hash,
//                std::string&& peer_id,
//                size_t port,
//                size_t uploaded,
//                size_t downloaded,
//                size_t left,
//                ushort compact)
//            : info_hash(std::move(info_hash)),
//            peer_id(std::move(peer_id)),
//            port(port),
//            uploaded(uploaded),
//            downloaded(downloaded),
//            left(left),
//            compact(compact)
//        {}
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
        std::cout << std::string{response.body.begin(), response.body.end()} << '\n'; // TODO: remove
        return std::string{response.body.begin(), response.body.end()};
    }

    bool try_extract_peers(const std::string& bencoded_peers, std::vector<std::string>& result)
    {
        auto decoded_peers_info = bencode::decode(bencoded_peers);
        const auto decoded_peers_info_dict = reinterpret_cast<BEncodedDictionary*>(&decoded_peers_info);

        auto search = decoded_peers_info_dict->find("peers");
        if (search == decoded_peers_info_dict->end())
        {
            std::cerr << "Missing peers in received value: \"" << bencoded_peers << "\"" << std::endl;
            return false;
        }

        std::string peers;
        search->second.try_get_string(peers);

        for (int i = 0; i < peers.size(); i += __k_peer_ip_byte_size)
        {
            std::stringstream ss;

//            ss << (bencoded_peers >> 40);
            // TODO: !!! bitwise !!!
        }

        return true;
    }
}