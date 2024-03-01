#pragma once

#include <memory>
#include <type_traits>

#include "message.h"
#include "peer_handshake.h"
#include "./net/tcp/tcp_client.interface.h"

// TODO: add descr
// TODO: copy/move
// TODO: send message as const void* with __attribute__((packed))
class PeerConnection
{
public:
    bool m_choked; // TODO: atomic, make private

    explicit PeerConnection(const std::shared_ptr<ITcpClient>& client_ptr, PeerHandshake&& handshake);

    PeerConnection(const PeerConnection& other) = delete;
    PeerConnection(PeerConnection&& other) noexcept;

    PeerConnection& operator=(const PeerConnection& other) = delete;
    PeerConnection& operator=(PeerConnection&& other) noexcept;

    void read_message()
    {
        auto client = m_client_ptr.lock();
        if (!client)
        {
            throw std::runtime_error("PeerConnection: unable to lock weak_ptr"); // TODO: separate
        }

        std::vector<uint8_t> response;
        auto status = client->get_response(response);
        success_or_throw(status, "read message error");

        // TODO: to function
        auto [message_id, payload] = parse_message(response);
        //

        switch (message_id)
        {
            case MessageId::choke:
                m_choked = true;
                break;
            case MessageId::unchoke:
                m_choked = false;
                break;
            case MessageId::bitfield:
                m_bitfield = bytes_to_integer<uint64_t>(payload.data());
//                m_bitfield = std::move((*static_cast<BitfieldMessage*>(&msg)).payload); TODO: remove
                break;
            case MessageId::have:
                // TODO:
                break;
            default:
                // TODO:
                break;
        }
    }

//    template <class TMessage, typename = std::enable_if_t<std::is_base_of_v<Message, TMessage> || std::is_same_v<TMessage, Message>>> TODO: remove
//    template <class TMessage, typename = std::enable_if_t<std::is_base_of_v<Message, TMessage>>>
//    TMessage read_message()
//    {
//        auto client = m_client_ptr.lock();
//        if (!client)
//        {
//            throw std::runtime_error("PeerConnection: unable to lock weak_ptr"); // TODO: separate
//        }
//
//        std::vector<uint8_t> response;
//        auto status = client->get_response(response);
//        success_or_throw(status, "read message error");
//
//        TMessage msg;
//        msg.deserialize(response);
//
//        switch (msg.id)
//        {
//            case MessageId::choke:
//                m_choked = true;
//                break;
//            case MessageId::unchoke:
//                m_choked = false;
//                break;
//            case MessageId::bitfield:
////                m_bitfield = std::move((*static_cast<BitfieldMessage*>(&msg)).payload);
//                break;
//            case MessageId::have:
//                // TODO:
//                break;
//            default:
//                // TODO:
//                break;
//        }
//
//        return msg;
//    }
//    template <class TMessage, typename = std::enable_if_t<std::is_base_of_v<Message, TMessage>>>
//    void read_message()
//    {
//        auto client = m_client_ptr.lock();
//        if (!client)
//        {
//            throw std::runtime_error("PeerConnection: unable to lock weak_ptr"); // TODO: separate
//        }
//
//        std::vector<uint8_t> response;
//        auto status = client->get_response(response);
//        success_or_throw(status, "read message error");
//
//        TMessage msg;
//        msg.deserialize(response);
//
//        switch (msg.id)
//        {
//            case MessageId::choke:
//                m_choked = true;
//                break;
//            case MessageId::unchoke:
//                m_choked = false;
//                break;
//            case MessageId::bitfield:
////                m_bitfield = std::move((*static_cast<BitfieldMessage*>(&msg)).payload);
//                break;
//            case MessageId::have:
//                // TODO:
//                break;
//            default:
//                // TODO:
//                break;
//        }
//
//        return msg;
//    }

    void send_interested() const;
    void send_unchoke() const; // TODO: remove???

private:
    std::string make_handshake(const std::shared_ptr<ITcpClient>& client);


    void set_peer_id(std::string&& peer_id) { m_peer_id = std::move(peer_id); }

    static void success_or_throw(const ssize_t& status, std::string&& message);

    static std::pair<MessageId, Payload> parse_message(const std::vector<uint8_t>& bytes);

    std::weak_ptr<ITcpClient> m_client_ptr;
    PeerHandshake m_handshake;
    std::string m_peer_id;

    uint64_t m_bitfield;
//    std::vector<uint8_t> m_bitfield; // TODO: remove
};