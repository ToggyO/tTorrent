#include "peer_connection.h"

PeerConnection::PeerConnection(const std::shared_ptr<ITcpClient>& client_ptr)
    : m_client_ptr{client_ptr}
{}

int PeerConnection::get_handshake(const PeerHandshake& handshake)
{
    auto client = m_client_ptr.lock();
    if (!client)
    {
        throw std::runtime_error("PeerConnection: unable to lock weak_ptr"); // TODO: separate
    }

    auto status = client->connect(handshake.get_domain(), handshake.get_port());
    if (status > 0) { return (int)status; }

    const auto handshake_message = handshake.get_protocol_message("00112233445566778899"); // TODO: generate client id

    std::vector<uint8_t> msg(handshake_message.begin(), handshake_message.end());
    status = client->send_request(msg);
    if (status > 0) { return (int)status; }

    std::vector<std::uint8_t> response;
    response.reserve(handshake.get_size() - 1);

    status = client->get_response(response);
    if (status > 0) { return (int)status; }

    auto handshake_result = std::string(response.begin(), response.end());

}