#include "processors.h"
#include "../lib/torrent/peers.h"

int process_peers(int argc, char* argv[])
{
    if (argc < 3)
    {
        std::cerr << "Usage: " << argv[0] << " info <torrent_file_path>" << std::endl;
        return 1;
    }

    std::string path = argv[2];
    std::string encoded_value;

    try
    {
        if (read_torrent_file_content(path, encoded_value) > 0)
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

        for (const auto& ip : peer_ips)
        {
            std::cout << ip << std::endl;
        }

        return 0;
    }
    catch (std::exception& ex)
    {
        std::cerr << "Failed to get peers info: " << ex.what() << std::endl;
        return 1;
    }
}