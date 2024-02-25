#pragma once

#include <memory>
#include <cstring>
#include <span>

#include "constants.h"
#include "./net/tcp/tcp_client.interface.h"
#include "utils.h"

// TODO: add descr
class PeerHandshake
{
    static inline uint8_t const INFO_HASH_STARTING_POSITION = 28;
    static inline uint8_t const PEER_ID_STARTING_POS = 48;
    static inline uint8_t const HASH_LEN = 20;
    static inline uint8_t const PROTOCOL_LEN = 19;
    static inline uint8_t const RESERVED_BYTES_COUNT = 8;

public:
    constexpr static uint8_t Size = 69; // 68 + \0

    constexpr static std::string_view Protocol = "BitTorrent protocol";

    PeerHandshake(std::string  domain, size_t port, std::string  info_hash, std::string  client_id);

    PeerHandshake(const PeerHandshake&) = default;
    PeerHandshake(PeerHandshake&&) noexcept = default;

    PeerHandshake& operator=(const PeerHandshake&) = default;
    PeerHandshake& operator=(PeerHandshake&&) noexcept = default;

    [[nodiscard]] const std::string& get_protocol_message() const;

    [[nodiscard]] const std::string& get_domain() const { return m_domain; }

    [[nodiscard]] const size_t& get_port() const { return m_port; }

    [[nodiscard]] const size_t& get_size() const { return m_port; }

    static std::string extract_peer_id(const std::string_view handshake_result)
    {
        return {handshake_result.substr(PEER_ID_STARTING_POS, HASH_LEN).data()};
    }

    bool verify_handshake(const PeerHandshake& other_handshake) const
    {
        return verify_handshake(other_handshake.get_protocol_message());
    }

    bool verify_handshake(const std::string_view other_handshake) const
    {
        return (other_handshake.substr(1, PROTOCOL_LEN) == Constants::protocol)
            && (to_hex(other_handshake.substr(INFO_HASH_STARTING_POSITION, HASH_LEN))  == m_info_hash);
    }

private:
    mutable bool m_is_handshake_cached;
    mutable std::string m_cached_handshake;

    std::string m_domain;
    size_t m_port;
    std::string m_info_hash;
    std::string m_client_id;
};