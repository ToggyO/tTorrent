#include "torrent_file.h"

TorrentFile::TorrentFile(const std::string_view file_path)
    : m_info_hash_is_set{false}
{
    std::string encoded_value;
    read_torrent_file_content(std::string(file_path), encoded_value);
    auto decoded_value = bencode::decode(encoded_value);
    m_content = std::move(*reinterpret_cast<BEncodedDictionary*>(&decoded_value));
}

std::string TorrentFile::get_announce() const
{
    std::string announce;
    if (!m_content.at(Constants::announce).try_get_string(announce))
    {
        announce = "";
    }
    return announce;
}

const std::string& TorrentFile::get_info_hash()
{
    if (m_info_hash_is_set) { return m_info_hash; }

    BEncodedDictionary info = get_info();

    auto encoded_info = bencode::encode(info);
    m_info_hash = std::move(compute_hash(encoded_info));

    m_info_hash_is_set = true;
    return m_info_hash;
}

Integer TorrentFile::get_file_size() const
{
    BEncodedDictionary info = get_info();
    Integer length;
    if (!info.at(Constants::file_size).try_get_int(length))
    {
        length = 0;
    }
    return length;
}

void TorrentFile::read_torrent_file_content(const std::string& file_path, std::string& content)
{
    std::ifstream file(file_path, std::ios::binary);
    if (!file.is_open())
    {
        throw std::runtime_error("Failed to open torrent file");
    }

    content = std::string(
            (std::istreambuf_iterator<char>(file)),
            (std::istreambuf_iterator<char>()));
}

BEncodedDictionary TorrentFile::get_info() const
{
    BEncodedDictionary info;
    if (!m_content.at(Constants::info).try_get_dictionary(info))
    {
        throw std::runtime_error("Torrent file doesn't contains 'info' section");
    }
    return info;
}