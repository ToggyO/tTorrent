#include "peer_handshake.h"

#include <utility>

PeerHandshake::PeerHandshake(std::string domain, size_t port, std::string info_hash, std::string client_id)
    : m_domain{std::move(domain)},
    m_port{port},
    m_info_hash{std::move(info_hash)},
    m_is_handshake_cached{false},
    m_cached_handshake(),
    m_client_id{std::move(client_id)}
{ }

const std::string& PeerHandshake::get_protocol_message() const
{
    if (m_is_handshake_cached)
    {
        return m_cached_handshake;
    }

    char msg[Size];
    msg[0] = PROTOCOL_LEN;
    std::strcpy(msg + 1, Protocol.data());
    std::memset(msg + HASH_LEN, 0, RESERVED_BYTES_COUNT);
    std::strcpy(msg + INFO_HASH_STARTING_POSITION, from_hex(m_info_hash).c_str());
    std::strcpy(msg + PEER_ID_STARTING_POS, m_client_id.data());

    m_is_handshake_cached = true;
    m_cached_handshake = std::string(msg, msg + (Size - 1));

    return m_cached_handshake;
}