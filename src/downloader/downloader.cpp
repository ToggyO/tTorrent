#include "downloader.h"

Downloader::Downloader(std::shared_ptr<INetInterface> &interface)
    : m_adapter{interface}
{}