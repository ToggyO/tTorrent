#include "processors.h"
#include "../downloader/downloader.h"
#include "../net_adapter/net_adapter.h"
#include "../lib/torrent/peers.h"

// TODO: duplicate
int process_download_piece(int argc, char* argv[])
{
    if (argc < 4) // TODO:
    {
        std::cerr << "Usage: " << argv[0] << " download_piece fsgldfkgoldfg" << std::endl; // TODO:
        return 1;
    }

    std::string output_file_path = argv[3];
    std::string torrent_file_path = argv[4];
    size_t piece_index = std::stoll(argv[5]);
    std::string encoded_value;

    try
    {
        if (read_torrent_file_content(torrent_file_path, encoded_value) > 0)
        {
            return 1;
        }

        auto decoded_value = bencode::decode(encoded_value);
        auto dictionary_ptr = reinterpret_cast<BEncodedDictionary*>(&decoded_value);

        std::string announce;
        dictionary_ptr->at("announce").try_get_string(announce);

        torrent::TorrentDownloadInfo download_info;
        if (auto search = dictionary_ptr->find("info"); search != dictionary_ptr->end())
        {
            auto encoded_info = bencode::encode(search->second);
            download_info.info_hash = std::move(encode_info_hash(compute_hash(encoded_info)));

            auto info_dictionary_ptr = reinterpret_cast<BEncodedDictionary*>(&(search->second));
            Integer length;
            info_dictionary_ptr->at("length").try_get_int(length);
            download_info.left = length;
        }

        // TODO: generate unique peer id
        download_info.peer_id = "00112233445566778899";
        //

        download_info.port = 6881; // TODO: check if port is free
        download_info.uploaded = 0;
        download_info.downloaded = 0;
        download_info.compact = 1;

        auto bencoded_peers = torrent::get_peers_info(std::move(announce), std::move(download_info));

        std::vector<std::string> peer_ips;
        torrent::try_extract_peers(bencoded_peers, peer_ips);

        std::shared_ptr<INetInterface> net_adapter = std::make_shared<NetAdapter>();
        Downloader downloader(net_adapter);

        std::string info_hash;
        if (auto search = dictionary_ptr->find("info"); search != dictionary_ptr->end())
        {
            auto encoded_info = bencode::encode(search->second);
            info_hash = std::move(compute_hash(encoded_info));
        }

        std::string handshake_result;
        const auto& [host, port] = split_inet_address(peer_ips[0]); // TODO: перебирать пиры
        auto status = downloader.get_peer_handshake(host, port, info_hash, handshake_result);
        if (status != 0)
        {
            throw std::runtime_error("Connection error"); // TODO: check result
        }
//        status = downloader.wait_for_bitfield()
    }
    catch (std::exception& ex)
    {
        std::cerr << "Failed to download piece. Index: " << piece_index << " Error: " << ex.what() << std::endl;
        return 1;
    }

    return 0;
}
