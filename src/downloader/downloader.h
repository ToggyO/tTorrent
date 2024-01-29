#pragma once

#include <cstring>
#include <memory>

#include "models.h"
#include "net.interface.h"
#include "../lib/torrent/utils.h"

// TODO: copy/move ctors
class Downloader
{
public:

    explicit Downloader(std::shared_ptr<INetInterface>& interface);

    auto get_peer_handshake(const std::string& domain, size_t port, const std::string& info_hash, std::string& result)
    {
//        net::Socket socket;
        auto adapter = m_adapter.lock();
        if (!adapter)
        {
            throw std::runtime_error("Downloader: net adapter cannot be null");
        }

        auto status = adapter->connect(domain, port);
        if (status > 0) { return (int)status; }

        const size_t len = 69; // 68 + \0
        char msg[len];
        msg[0] = 19;
//        std::strcpy(msg + 1, __k_protocol.data()); TODO: !!!
        std::strcpy(msg + 1, "BitTorrent protocol");
        std::memset(msg + 20, 0, 8);
        std::strcpy(msg + 28, from_hex(info_hash).c_str());
        std::strcpy(msg + 48, "00112233445566778899");

        std::vector<uint8_t> bytes(msg, msg + len);
        status = adapter->send_request(bytes);
        if (status > 0) { return (int)status; }

        std::vector<std::uint8_t> response;
        response.reserve(len - 1);

        status = adapter->get_response(response);
        if (status > 0) { return (int)status; }

        result = std::string(response.begin(), response.end());
        return 0;
    }

    // TODO: separate
    // TODO: multithreading
    void download() const
    {
        if (!m_handshake_done)
        {
            throw std::runtime_error(""); // TODO: error message
        }

    }

//    auto send_bitfield(const BitfieldPeerMessage& message) const
    auto wait_for_bitfield() const
    {
        auto adapter = m_adapter.lock();
        if (!adapter)
        {
            throw std::runtime_error("Downloader: net adapter cannot be null");
        }

        std::vector<uint8_t> response;
        auto status = adapter->get_response(response);
        if (status > 0)
        {
            return status;
        }



//        const BitfieldPeerMessage message{};
//        auto ptr = reinterpret_cast<const char*>(&message);
//        std::vector<uint8_t> bytes(ptr, ptr + sizeof(message));
//        auto status = adapter->send_request(bytes);
//        if (status > 0)
//        {
//            return status;
//        }


        return (ssize_t)0;
    }

private:
    std::weak_ptr<INetInterface> m_adapter;
    bool m_handshake_done; // TODO: atomic


};