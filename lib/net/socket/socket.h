#pragma once

#if defined(_WIN32) || defined(_WIN64)
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#include <winsock2.h>
#include <ws2tcpip.h>
#endif
#if defined(linux) || defined(__linux__) || defined(__linux)
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#endif
#if defined(__APPLE__) && defined(__MACH__)
// TODO: OSX implementation
#endif
#include <optional>
#include <span>
#include <stdexcept>
#include <vector>

// TODO: disconnect?
namespace net
{
    constexpr static size_t BUFFER_SIZE = 4096;
    constexpr auto socket_creation_failed { -1 };

    using file_descriptor = int;

    // TODO: check win implementation
    // TODO: add descr
    // TODO: make thread safe
    class Socket
    {
    public:
#if defined(_WIN32) || defined(_WIN64)
        Socket() : m_is_connected{false}
        {
            WSADATA wsaData;
            auto sock_creation_result = WSAStartup(MAKEWORD(2,2), &wsaData);
#elif defined(linux) || defined(__linux__) || defined(__linux)
        explicit Socket(int domain = AF_INET, int type = SOCK_STREAM, int protocol = 0)
            : m_domain{domain},
            m_is_connected{false}
        {
            auto sock_creation_result = socket(domain, type, protocol);
#else
            throw std::runtime_error("Unsupported platform");
#endif
            if (sock_creation_result == socket_creation_failed)
            {
                throw std::runtime_error("Error creating socket");
            }

            int timeout = 5; // TODO: HADRCODE

            timeval tv{};
            tv.tv_sec = timeout;
            tv.tv_usec = 0; // TODO: таймаут чет не робит! Проверить

            setsockopt(sock_creation_result, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(struct timeval));
            setsockopt(sock_creation_result, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(struct timeval));

            m_socket = sock_creation_result;
        }

        Socket(const Socket&) = delete;
        Socket(Socket&& other) noexcept
            : m_socket{other.m_socket},
            m_domain{other.m_domain},
            m_is_connected{other.m_is_connected}
        {
            other.m_socket = std::nullopt;
            other.m_domain = 0;
            other.m_is_connected = false;
        }

        ~Socket() { close_if_necessary(); }

        Socket& operator=(const Socket&) = delete;
        Socket& operator=(Socket&& other) noexcept
        {
            if (other.m_socket != m_socket)
            {
                close_if_necessary();
                m_socket = other.m_socket;
                m_domain = other.m_domain;
                m_is_connected = other.m_is_connected;

                other.m_socket = std::nullopt;
                other.m_domain = 0;
                other.m_is_connected = false;
            }
            return *this;
        }

        ssize_t connect_to(std::string_view host, size_t port)
        {
            sockaddr_in hint{};
            hint.sin_family = m_domain;
            hint.sin_port = htons(port);

            if (inet_pton(AF_INET, host.data(), &hint.sin_addr) <= 0)
            {
                std::cerr << "Error converting IP address" << std::endl;
                return 1;
            }

            ssize_t connect_result = ::connect(m_socket.value(), (struct sockaddr*)&hint, sizeof(hint));
            if (connect_result == -1)
            {
                std::cerr << "Error connecting to the server: " << errno << std::endl;
                return 1;
            }

            m_is_connected = true;
            return 0;
        }

        ssize_t send(const void* buffer, const size_t length) const
        {
            if (!m_is_connected)
            {
                std::cerr << "Not connected" << std::endl;
                return 1;
            }

//            std::vector<unsigned char> payload;
//            payload.reserve(message_length);
//
//            std::span<unsigned char> span(reinterpret_cast<unsigned char*>(message_ptr), message_length);
//            std::copy(span.begin(), span.end(), std::back_insert_iterator<std::vector<unsigned char>>(payload));

//            auto response = ::send(m_socket.value(), payload.data(), payload.size(), 0); TODO: remove
            auto response = ::send(m_socket.value(), reinterpret_cast<const char*>(buffer), length, 0);
            if (response == -1)
            {
                std::cerr << "Error sending data" << std::endl;
                return 1;
            }
            return 0;
        }

        ssize_t receive(std::vector<std::uint8_t>& result) const
        {
            if (!m_is_connected)
            {
                std::cerr << "Not connected" << std::endl;
                return 1;
            }

            char buffer[BUFFER_SIZE] = {0};
            ssize_t received;
//            do TODO: HANDLE LOOP
//            {
                received = recv(m_socket.value(), buffer, sizeof(buffer), 0);
                if (received < 0)
                {
                    std::cerr << "Error receiving data" << std::endl;
                    return 1;
                }

                result.insert(result.end(), buffer, buffer + received);
//            } while (received > 0);

            return 0;
        }

    private:
        std::optional<file_descriptor> m_socket;
        int m_domain;
        bool m_is_connected; // TODO: ATOMIC

        void close_if_necessary()
        {
            ssize_t socket_close_result;
            if (m_socket.has_value())
            {
#if defined(_WIN32) || defined(_WIN64)
                socket_close_result = closesocket(m_socket);
                WSACleanup();
#endif
#if defined(linux) || defined(__linux__) || defined(__linux) || defined(__APPLE__) && defined(__MACH__)
                socket_close_result = close(m_socket.value());
#endif
                if (socket_close_result == socket_creation_failed) {
                     throw std::system_error(errno, std::system_category(), "Failed to cloe socket ");
                }
            }
        }
    };
}
