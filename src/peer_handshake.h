#pragma once

#include <memory>
#include <cstring>

#include "./net/tcp/tcp_client.interface.h"
#include "utils.h"

// TODO: move/copy
// TODO: add descr
class PeerHandshake
{
public:
    constexpr static uint8_t Size = 69; // 68 + \0

    constexpr static std::string_view Protocol = "BitTorrent protocol";

    PeerHandshake(std::string&& domain, size_t port, std::string&& info_hash, std::string client_id);

    [[nodiscard]] const std::string& get_protocol_message() const;

    [[nodiscard]] const std::string& get_domain() const { return m_domain; }

    [[nodiscard]] const size_t& get_port() const { return m_port; }

    [[nodiscard]] const size_t& get_size() const { return m_port; }

    bool verify_handshake(const PeerHandshake& other_handshake) const
    {
        return verify_handshake(other_handshake.get_protocol_message());
    }

    bool verify_handshake(const std::string_view other_handshake) const
    {
        return get_protocol_message() == other_handshake;
    }

private:
    mutable bool m_is_handshake_cached;
    mutable std::string m_cached_handshake;

    std::string m_domain;
    size_t m_port;
    std::string m_info_hash;
    std::string m_client_id;
    constexpr static uint8_t m_len = 19;
    constexpr static uint8_t m_reserved_bytes_count = 8;
};