#pragma once

#include <cstdint>
#include <string_view>
#include <vector>

// TODO: add descr
class INetInterface
{
public:
    virtual ssize_t connect(std::string_view host, size_t port) = 0;

    virtual ssize_t send_request(const std::vector<uint8_t>& payload) const = 0;

    virtual ssize_t get_response(std::vector<std::uint8_t>& result) const = 0;

    virtual ~INetInterface() {}
};