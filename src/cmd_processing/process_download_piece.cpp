#include "processors.h"
#include "../torrent_client.h"
#include "../torrent_file.h"
#include "../peer_manager.h"
#include "../net/http/http_client.h"

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
        TorrentFile torrent_file(torrent_file_path);

        // TODO: generate unique peer id
        auto client_id = "00112233445566778899";
        //

        auto encoded_info_hash = encode_info_hash(std::string(torrent_file.get_info_hash()));
        auto http_client_ptr = std::make_shared<HttpClient>(HttpClient{});
        PeerManager peer_manager(
                http_client_ptr,
                std::move(torrent_file.get_announce()),
                std::move(encoded_info_hash),
                client_id,
                Constants::client_port,
                torrent_file.get_file_size());

        auto peers = peer_manager.get_peers(0); // TODO: check bytes downloaded
        std::cout << "kek" << std::endl;
//        std::vector<std::string> peer_ips;
//        torrent::try_extract_peers(bencoded_peers, peer_ips);

//        std::shared_ptr<INetInterface> net_adapter = std::make_shared<NetAdapter>();
//        TorrentClient torrent(net_adapter);
//
//        std::string info_hash;
//        if (auto search = dictionary_ptr->find("info"); search != dictionary_ptr->end())
//        {
//            auto encoded_info = bencode::encode(search->second);
//            info_hash = std::move(compute_hash(encoded_info));
//        }
//
//        std::string handshake_result;
//        const auto& [host, port] = split_inet_address(peer_ips[0]); // TODO: перебирать пиры
//        auto status = torrent.get_peer_handshake(host, port, info_hash, handshake_result);
//        if (status != 0)
//        {
//            throw std::runtime_error("Connection error"); // TODO: check result
//        }
//        torrent.download();
//        status = torrent_client.wait_for_bitfield()
    }
    catch (std::exception& ex)
    {
        std::cerr << "Failed to download piece. Index: " << piece_index << " Error: " << ex.what() << std::endl;
        return 1;
    }

    return 0;
}
