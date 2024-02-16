#include "peer_handshake.h"

PeerHandshake::PeerHandshake(const std::string& domain, size_t port, const std::string& info_hash, const std::string& client_id)
    : m_domain{domain},
    m_port{port},
    m_info_hash{info_hash},
    m_is_handshake_cached{false},
    m_cached_handshake(),
    m_client_id{client_id}
{ }

const std::string& PeerHandshake::get_protocol_message() const
{
    if (m_is_handshake_cached)
    {
        return m_cached_handshake;
    }

    char msg[Size];
    msg[0] = m_len;
    std::strcpy(msg + 1, Protocol.data());
    std::memset(msg + 20, 0, m_reserved_bytes_count);
    std::strcpy(msg + 28, from_hex(m_info_hash).c_str());
    std::strcpy(msg + 48, m_client_id.data());

    m_is_handshake_cached = true;
    m_cached_handshake = std::string(msg, msg + 68);

    return m_cached_handshake;
}