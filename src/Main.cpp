#include <iostream>
#include <string>

#include "lib/bencode/decode.h"
#include "lib/bencode/encode.h"
#include "lib/json/json_bencode.h"
#include "lib/torrent/meta.h"
#include "lib/torrent/utils.h"

int process_decode(int argc, char* argv[]);

int process_info(int argc, char* argv[]);

int process_peers(int argc, char* argv[]);

// todo: вынести
enum class Commands
{
    decode,
    info,
    peers
};

// TODO: постараться разбить на h/cpp. Выделить либы
int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        // TODO: доплнить
        std::cerr << "Usage: " << argv[0] << " <command> <encoded_value>" << std::endl;
        return 1;
    }

    std::string command = argv[1];

    std::unordered_map<std::string, Commands> command_mapper = {
        { "decode", Commands::decode },
        { "info", Commands::info },
        { "peers", Commands::peers }
    };

    auto k = command_mapper[command]; // TODO: сделать безопаснее
    switch (k)
    {
        case Commands::decode:
            return process_decode(argc, argv);
        case Commands::info:
            return process_info(argc, argv);
        case Commands::peers:
            return process_peers(argc, argv);
        default:
            // TODO:

    }

    std::cerr << "Unknown command: " << command << std::endl;
    return 1;
}

int process_decode(int argc, const char* argv[])
{
    // TODO: try...catch
    if (argc < 3)
    {
        std::cerr << "Usage: " << argv[0] << " decode <encoded_value>" << std::endl;
        return 1;
    }

    std::string encoded_value = argv[2];
    auto decoded_value = bencode::decode(encoded_value);
    std::cout << ((json)decoded_value).dump() << std::endl;
    return 0;
}

int process_info(int argc, char* argv[])
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
        auto torrent_meta = torrent::Torrent::from_bencode(*dictionary_ptr);

        // TODO: refactor output
        std::cout << "Tracker URL: " << torrent_meta.announce << std::endl
                  << "Length: " << torrent_meta.length << std::endl;

        if (auto search = dictionary_ptr->find("info"); search != dictionary_ptr->end())
        {
            auto encoded_info = bencode::encode(search->second);
            std::cout << "Info Hash: " << compute_hash(encoded_info) << std::endl;
        }

        std::cout << "Piece Length: " << torrent_meta.piece_length << std::endl;
        std::cout << "Piece Hashes:" << std::endl;
        for (const auto& hash : get_peaces_hashes(torrent_meta.pieces))
        {
            std::cout << hash << std::endl;
        }

        return 0;
    }
    catch (...)
    {
        std::cerr << "Invalid torrent file: " << path << std::endl;
        return 1;
    }
}

int process_peers(int argc, char* argv[])
{

}