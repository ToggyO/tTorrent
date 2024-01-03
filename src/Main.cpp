#include <iostream>
#include <fstream>
#include <string>

#include "lib/bencode/bencode.h"
#include "lib/torrent/meta.h"

int read_torrent_file_content(const std::string& path, std::string& content)
{
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open())
    {
        std::cerr << "Failed to open torrent file";
        return 1;
    }

    content = std::string(
            (std::istreambuf_iterator<char>(file)),
            (std::istreambuf_iterator<char>()));
    return 0;
}

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
        if (argc < 3)
        {
            std::cerr << "Usage: " << argv[0] << " decode <encoded_value>" << std::endl;
            return 1;
        }

        // Uncomment this block to pass the first stage
        std::string encoded_value = argv[2];
        json decoded_value = bencode::decode(encoded_value);
        std::cout << decoded_value.dump() << std::endl;
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
        std::string encoded_value;
        if (read_torrent_file_content(path, encoded_value) > 0)
        {
            return 1;
        }

        json decoded_value = bencode::decode(encoded_value);
        auto torrent_meta = torrent::convert_to_torrent_meta(decoded_value);

        // TODO: refactor output
        std::cout << "Tracker: " << torrent_meta.announce << std::endl
            << "Length: " << torrent_meta.length << std::endl;

        return 0;
    }

    std::cerr << "unknown command: " << command << std::endl;
    return 1;
}