#include "net_adapter.h"

ssize_t NetAdapter::connect(std::string_view host, size_t port)
{
    return connect_to(std::forward<std::string_view>(host), std::forward<size_t>(port));
}


ssize_t NetAdapter::send_request(const std::vector<uint8_t>& payload) const
{
    return send(payload.data(), payload.size());
}

ssize_t NetAdapter::get_response(std::vector<std::uint8_t>& result) const
{
    return receive(result);
}