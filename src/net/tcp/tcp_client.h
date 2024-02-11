#pragma once

#include "tcp_client.interface.h"
#include "../../../lib/net/socket/socket.h"

// TODO: add descr
class TcpClient : public net::Socket, public ITcpClient
{
public:
    TcpClient() = default;

    virtual ssize_t connect(std::string_view host, size_t port) final;

    virtual ssize_t send_request(const std::vector<uint8_t>& payload) const final;

    virtual ssize_t get_response(std::vector<std::uint8_t>& result) const final;
};