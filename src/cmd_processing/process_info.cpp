#include "processors.h"
#include "../lib/torrent/meta.h"

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