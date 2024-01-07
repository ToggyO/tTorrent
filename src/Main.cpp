#include <iostream>
#include <string>

#include "lib/bencode/decode.h"
#include "lib/bencode/encode.h"
#include "lib/json/json_bencode.h"
#include "lib/torrent/meta.h"
#include "lib/torrent/utils.h"

void process_decode(const std::string&);

int process_info(const std::string&);

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        // TODO: доплнить
        std::cerr << "Usage: " << argv[0] << " <command> <encoded_value>" << std::endl;
        return 1;
    }

    std::string command = argv[1];

    if (command == "decode")
    {
        // TODO: try...catch
        if (argc < 3)
        {
            std::cerr << "Usage: " << argv[0] << " decode <encoded_value>" << std::endl;
            return 1;
        }

        std::string encoded_value = argv[2];
        process_decode(encoded_value);

        return 0;
    }

    if (command == "info")
    {
        if (argc < 3)
        {
            std::cerr << "Usage: " << argv[0] << " info <torrent_file_path>" << std::endl;
            return 1;
        }

        std::string path = argv[2];
        try
        {
            return process_info(path);
        }
        catch (...)
        {
            std::cerr << "Invalid torrent file: " << path << std::endl;
            return 1;
        }
    }

    std::cerr << "Unknown command: " << command << std::endl;
    return 1;
}

void process_decode(const std::string& encoded_value)
{
    auto decoded_value = bencode::decode(encoded_value);
    std::cout << ((json)decoded_value).dump() << std::endl;
}

int process_info(const std::string& path)
{
    std::string encoded_value;
    if (read_torrent_file_content(path, encoded_value) > 0)
    {
        return 1;
    }

    auto decoded_value = bencode::decode(encoded_value);
    auto dictionary_ptr = reinterpret_cast<BEncodedDictionary*>(&decoded_value);
    auto torrent_meta = torrent::convert_to_torrent_meta(*dictionary_ptr);

    // TODO: refactor output
    std::cout << "Tracker URL: " << torrent_meta.announce << std::endl
              << "Length: " << torrent_meta.length << std::endl;

    if (auto search = dictionary_ptr->find("info"); search != dictionary_ptr->end())
    {
        auto encoded_info = bencode::encode(search->second);
        std::cout << "Info Hash: " << compute_hash(encoded_info) << std::endl;
    }

    return 0;
}