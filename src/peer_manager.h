#pragma once

#include <exception>
#include <memory>
#include <string>
#include <vector>

#include "constants.h"
#include "utils.h"
#include "net/http/http_client.interface.h"
#include "../lib/bencode/decode.h" // TODO: remove ..

using namespace bencode;

struct Peer
{
    std::string host;
    int port;

    [[nodiscard]] std::string to_string() const;
};

// TODO: copy/move
// TODO: add descr
class PeerManager
{
public:
    PeerManager(const std::shared_ptr<IHttpClient>& http_client,
        std::string&& announce,
        std::string&& info_hash,
        std::string&& client_id,
        ssize_t port,
        ssize_t file_size);

    std::vector<std::unique_ptr<Peer>> get_peers(ssize_t bytes_downloaded = 0, bool compact = true);

private:
    static void extract_peers(const std::string& bencoded_peers, std::vector<std::unique_ptr<Peer>>& result);

    std::weak_ptr<IHttpClient> m_client_ptr;
    std::string m_announce;
    std::string m_info_hash;
    std::string m_client_id;
    ssize_t m_port;
    ssize_t m_file_size;
    ssize_t m_uploaded; // TODO: handle uploaded
};