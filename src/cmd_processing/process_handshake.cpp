#include "processors.h"

#include "../lib/torrent/peers.h"

std::pair<std::string, size_t> split_inet_address(const std::string_view address)
{
    size_t colon_index = address.find( __k_colon, 0);
    if (colon_index == std::string::npos)
    {
        throw std::runtime_error("Invalid inet address: " + std::string(address));
    }

    auto host = address.substr(0, colon_index);
    auto port = std::stoll(std::string(address.substr(colon_index + 1)));
    return std::pair<std::string, size_t>(host, port);
}

// TODO: чекнуть на дупликации process_peers, process_info
int process_handshake(int argc, char* argv[])
{
    if (argc < 4)
    {
        std::cerr << "Usage: " << argv[0] << " info <torrent_file_path>" << std::endl;
        return 1;
    }

    // TODO: validate <peer_ip>:<peer_port>
    // argv[3]
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
//            info_hash = std::move(encode_info_hash(compute_hash(encoded_info)));
            info_hash = std::move(compute_hash(encoded_info));
        }
        // TODO: validate info_hash

        std::string peer_id;
        const auto& [host, port] = split_inet_address(argv[3]);
        int handshake_result = torrent::make_peer_handshake(host, port, info_hash, peer_id);
        if (handshake_result != 0)
        {
            throw std::runtime_error("Connection error");
        }

        std::cout << "Peer ID: " << peer_id << std::endl;
    }
    catch (std::exception& ex)
    {
        std::cerr << "Failed to make handshake: " << ex.what() << std::endl;
        return 1;
    }

    return 0;
}