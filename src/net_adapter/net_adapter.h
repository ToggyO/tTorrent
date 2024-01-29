#pragma once

#include "../downloader/net.interface.h"
#include "../lib/net/socket/socket.h"

// TODO: add descr
class NetAdapter : public net::Socket, public INetInterface
{
public:
    NetAdapter() = default;

    virtual ssize_t connect(std::string_view host, size_t port) final;

    virtual ssize_t send_request(const std::vector<uint8_t>& payload) const final;

    virtual ssize_t get_response(std::vector<std::uint8_t>& result) const final;
};