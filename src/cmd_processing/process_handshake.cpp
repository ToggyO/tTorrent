#include "processors.h"

#include "../../lib/torrent/peers.h" // TODO: remove ..

int process_handshake(int argc, char* argv[])
{
    if (argc < 4)
    {
        std::cerr << "Usage: " << argv[0] << " handshake <torrent_file_path> <peer_ip>:<peer_port>" << std::endl;
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

        std::string info_hash;
        if (auto search = dictionary_ptr->find("info"); search != dictionary_ptr->end())
        {
            auto encoded_info = bencode::encode(search->second);
            info_hash = std::move(compute_hash(encoded_info));
        }

        std::string handshake_result;
        const auto& [host, port] = split_inet_address(argv[3]);
        int result_code = torrent::get_peer_handshake(host, port, info_hash, handshake_result);
        if (result_code != 0)
        {
            throw std::runtime_error("Connection error");
        }

        auto peer_id = to_hex(handshake_result.substr(48, 20));
        std::cout << "Peer ID: " << peer_id << std::endl;
    }
    catch (std::exception& ex)
    {
        std::cerr << "Failed to make handshake: " << ex.what() << std::endl;
        return 1;
    }

    return 0;
}