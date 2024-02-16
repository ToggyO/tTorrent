#include "processors.h"
#include "../peer_connection.h"
#include "../peer_handshake.h"
#include "../peer_manager.h"
#include "../torrent_client.h"
#include "../torrent_file.h"
#include "../net/http/http_client.h"
#include "../net/tcp/tcp_client.h"

#include "../../lib/torrent/peers.h"

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

        auto info_hash = std::string(torrent_file.get_info_hash());
        auto encoded_info_hash = encode_info_hash(info_hash);
        auto http_client_ptr = std::make_shared<HttpClient>(HttpClient{});
        PeerManager peer_manager(
                http_client_ptr,
                std::move(torrent_file.get_announce()),
                std::move(encoded_info_hash),
                client_id,
                Constants::client_port,
                torrent_file.get_file_size());

        auto peers = peer_manager.get_peers(0); // TODO: check bytes downloaded

        auto tcp_client_ptr = std::make_shared<TcpClient>(TcpClient{});
        PeerConnection connection(tcp_client_ptr);

        // TODO: handle multiple peers
//    "00112233445566778899" // TODO: generate client id
//        PeerHandshake handshake(peers[0]->host, peers[0]->port, info_hash, "00112233445566778899");
//        auto handshake_result = connection.get_handshake(handshake);
//        if (!handshake.verify_handshake(handshake_result))
//        {
//            throw std::runtime_error("Invalid handshake result: " + handshake_result);
//        }



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
        std::string handshake_result;
//        const auto& [host, port] = split_inet_address(peer_ips[0]); // TODO: перебирать пиры
        auto status = torrent::get_peer_handshake(peers[0]->host, peers[0]->port, info_hash, handshake_result);
        if (status != 0)
        {
            throw std::runtime_error("Connection error"); // TODO: check result
        }
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
