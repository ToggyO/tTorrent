#pragma once

#include <memory>

#include "peer_handshake.h"
#include "./net/tcp/tcp_client.interface.h"

class PeerConnection
{
public:
    explicit PeerConnection(const std::shared_ptr<ITcpClient>& client_ptr);

    // TODO: check return type
    std::string get_handshake(const PeerHandshake& handshake);

private:
    static void success_or_throw(const ssize_t& status, std::string&& message);

    std::weak_ptr<ITcpClient> m_client_ptr;
};