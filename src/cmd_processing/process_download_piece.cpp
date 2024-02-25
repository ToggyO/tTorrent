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

        for (const auto& peer : peers) // TODO: multithreaded, work queue
        {
            try {
                // TODO: handle multiple peers
//    "00112233445566778899" // TODO: generate client id
                PeerConnection connection(tcp_client_ptr, PeerHandshake(peers[1]->host, peers[1]->port, info_hash, "00112233445566778899"));
                // TODO: if bitfield is satisfied - send interested
                connection.send_interested();

                connection.send_unchoke();
                connection.read_message<UnchokeMessage>();
                if (connection.m_choked) {
                    continue;
                }

                connection.read_message<BitfieldMessage>(); // TODO: handle payload bitfield - pieces that peer contains

//                // TODO: if bitfield is satisfied - send interested
//                connection.send_interested();

                auto unchoke = connection.read_message<UnchokeMessage>(); // TODO: check choke or unchoke
            }
            catch (...)
            {
                continue; // TODO: check
            }
        }
        std::cout << "kek" << std::endl;
    }
    catch (std::exception& ex)
    {
        std::cerr << "Failed to download piece. Index: " << piece_index << " Error: " << ex.what() << std::endl;
        return 1;
    }

    return 0;
}
