#include "peer_manager.h"

std::string Peer::to_string() const
{
    std::stringstream formatter;
    formatter << host << ':' << port;
    return formatter.str();
}

PeerManager::PeerManager(
    const std::shared_ptr<IHttpClient>& http_client,
    std::string&& announce,
    std::string&& info_hash,
    std::string&& client_id,
    ssize_t port,
    ssize_t file_size)
    : m_client_ptr{http_client}, // TODO: check copying!!!!!!!
    m_announce{std::move(announce)},
    m_info_hash{std::move(info_hash)},
    m_client_id{std::move(client_id)},
    m_port{port},
    m_file_size{file_size},
    m_uploaded{0}
{}

std::vector<std::unique_ptr<Peer>> PeerManager::get_peers(const ssize_t bytes_downloaded, bool compact)
{
    auto client = m_client_ptr.lock();
    if (!client)
    {
        throw std::runtime_error("PeerManager: unable to lock weak_ptr"); // TODO: separate
    }

    std::stringstream ss;
    ss << m_announce << Constants::question_char
       << Constants::info_hash << Constants::equality_sign_char << m_info_hash << Constants::ampersand_char
       << Constants::peer_id << Constants::equality_sign_char << m_client_id << Constants::ampersand_char
       << Constants::port << Constants::equality_sign_char << m_port << Constants::ampersand_char
       << Constants::uploaded << Constants::equality_sign_char << m_uploaded << Constants::ampersand_char
       << Constants::downloaded << Constants::equality_sign_char << bytes_downloaded << Constants::ampersand_char
       << Constants::left << Constants::equality_sign_char << m_file_size - bytes_downloaded << Constants::ampersand_char
       << Constants::compact << Constants::equality_sign_char << (compact ? 1 : 0);

    auto response = client->send(ss.str());
    if (response.status == 0)
    {
        throw std::runtime_error("Failed to discover peers");
    }

    std::vector<std::unique_ptr<Peer>> peers;
    extract_peers(std::string{response.body.begin(), response.body.end()}, peers);

    return peers;
}

void PeerManager::extract_peers(const std::string& bencoded_peers, std::vector<std::unique_ptr<Peer>>& result)
{
    auto decoded_peers_info = decode(bencoded_peers);
    const auto decoded_peers_info_dict = reinterpret_cast<BEncodedDictionary*>(&decoded_peers_info);

    auto search = decoded_peers_info_dict->find("peers");
    if (search == decoded_peers_info_dict->end())
    {
        throw std::runtime_error("Missing peers in received value: \"" + bencoded_peers + "\"\n");
    }

    std::string peers;
    search->second.try_get_string(peers);

    std::stringstream ss;
    for (int i = 0; i < peers.size(); i += Constants::peer_ip_byte_size)
    {
        ss.clear();
        ss.str("");

        ushort port = ((unsigned short)peers[i + 4] << 8) | (unsigned char)peers[i + 5];
        ss
            << std::to_string((unsigned char)peers[i]) << Constants::point_char
            << std::to_string((unsigned char)peers[i + 1]) << Constants::point_char
            << std::to_string((unsigned char)peers[i + 2]) << Constants::point_char
            << std::to_string((unsigned char)peers[i + 3]);

        result.push_back(std::make_unique<Peer>(Peer{ss.str(), port}));
    }
}