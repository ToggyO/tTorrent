#include "torrent_client.h"

TorrentClient::TorrentClient(std::shared_ptr<ITcpClient> &interface)
    : m_adapter{interface},
    m_handshake_done{false}
{}