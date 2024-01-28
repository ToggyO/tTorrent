#pragma once

#if defined(_WIN32) || defined(_WIN64)
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#include <winsock2.h>
#include <ws2tcpip.h>
#endif
#if defined(linux) || defined(__linux__) || defined(__linux) || defined(__APPLE__) && defined(__MACH__)
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#endif
#include <stdexcept>
#include <vector>

namespace net
{
    constexpr static size_t BUFFER_SIZE = 4096;

    // TODO: move/copy ctors/operators
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
            m_socket = WSAStartup(MAKEWORD(2,2), &wsaData);
#elif defined(linux) || defined(__linux__) || defined(__linux) || defined(__APPLE__) && defined(__MACH__)
        explicit Socket(int domain = AF_INET, int type = SOCK_STREAM, int protocol = 0)
            : m_domain{domain},
            m_is_connected{false}
        {
            m_socket = socket(domain, type, protocol);
#else
            throw std::runtime_error("Unsupported platform");
#endif
            if (m_socket == -1)
            {
                throw std::runtime_error("Error creating socket");
            }
        }

        ~Socket()
        {
#if defined(_WIN32) || defined(_WIN64)
            closesocket(m_socket);
            WSACleanup();
#endif
#if defined(linux) || defined(__linux__) || defined(__linux) || defined(__APPLE__) && defined(__MACH__)
            close(m_socket);
#endif
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

            ssize_t connect_result = ::connect(m_socket, (struct sockaddr*)&hint, sizeof(hint));
            if (connect_result == -1)
            {
                std::cerr << "Error connecting to the server: " << errno << std::endl;
                return 1;
            }

            m_is_connected = true;
            return 0;
        }

//        ssize_t send(std::string_view message) const TODO: check
//        ssize_t send(const std::string& message) const
        ssize_t send(const char* message, size_t message_length) const
        {
            if (!m_is_connected)
            {
                std::cerr << "Not connected" << std::endl;
                return 1;
            }

//            auto response = ::send(m_socket, message.data(), message.size(), 0);
            auto response = ::send(m_socket, message, message_length, 0);
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
//            do
//            {
                received = recv(m_socket, buffer, sizeof(buffer), 0);
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
        int m_socket;
        int m_domain;
        bool m_is_connected; // TODO: ATOMIC
    };
}
