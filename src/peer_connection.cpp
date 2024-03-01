#include "peer_connection.h"

PeerConnection::PeerConnection(const std::shared_ptr<ITcpClient>& client_ptr, PeerHandshake&& handshake)
    : m_client_ptr{client_ptr},
      m_handshake(std::move(handshake)),
      m_choked{false},
      m_bitfield{0}
{

}

bool PeerConnection::establish_connection()
{
    try
    {
        connect();
        receive_bitfield();
        send_interested(); // TODO: check
        return true;
    }
    catch (std::exception &e)
    {
        // TODO: handle
        return false;
    }
}

void PeerConnection::connect()
{
    auto client = m_client_ptr.lock();
    if (!client)
    {
        throw std::runtime_error("PeerConnection: unable to lock weak_ptr"); // TODO: separate
    }

    auto status = client->connect(m_handshake.get_domain(), m_handshake.get_port());
    success_or_throw(status, "connection error");

    auto handshake_result = make_handshake(client);
    if (!m_handshake.verify_handshake(handshake_result))
    {
        throw std::runtime_error("Invalid handshake result: " + handshake_result);
    }

    set_peer_id(PeerHandshake::extract_peer_id(handshake_result));
}

bool PeerConnection::receive_bitfield()
{
    read_message();
    if
}

void PeerConnection::send_interested() const
{
    auto client = m_client_ptr.lock();
    if (!client)
    {
        throw std::runtime_error("PeerConnection: unable to lock weak_ptr"); // TODO: separate
    }

    auto status = client->send_request(InterestedMessage().serialize());
    success_or_throw(status, "send interested error");
}

void PeerConnection::send_unchoke() const
{
    auto client = m_client_ptr.lock();
    if (!client)
    {
        throw std::runtime_error("PeerConnection: unable to lock weak_ptr"); // TODO: separate
    }

    auto status = client->send_request(UnchokeMessage().serialize());
    success_or_throw(status, "send unchoke error");
}

std::string PeerConnection::make_handshake(const std::shared_ptr<ITcpClient>& client)
{
    const auto& handshake_message = m_handshake.get_protocol_message();

    std::vector<uint8_t> msg(handshake_message.begin(), handshake_message.end());
    auto status = client->send_request(msg);
    success_or_throw(status, "request error");

    std::vector<std::uint8_t> response;
    response.reserve(m_handshake.get_size() - 1);

    status = client->get_response(response);
    success_or_throw(status, "response error");

    return {response.begin(), response.end()};
}

void PeerConnection::success_or_throw(const ssize_t& status, std::string&& message)
{
    if (status > 0)
    {
        throw std::runtime_error("PeerConnection: " + message + ". Status: " + std::to_string(status));
    }
}

// TODO: мбыть сделать класс Response, где внутри парсить ответ
// Таким образом вынесем знание об обработки сообщений из класса, характеризующего соединение с пиром
std::pair<MessageId, Payload> PeerConnection::parse_message(const std::vector<uint8_t>& bytes)
{
    if (bytes.size() < 5)
    {
        throw std::runtime_error("Error parsing message: total size less than 5");
    }

    auto length = ((uint32_t)bytes[0] << 24) | (bytes[1] << 16) | (bytes[2] << 8) | bytes[3];

    auto begin = bytes.begin();
    std::vector<uint8_t> payload(begin + 5, begin + 5 + (length - 1));

    return std::make_pair(MessageId(bytes[4]), std::move(payload));
}