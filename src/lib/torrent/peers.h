#pragma once

#include <string>

#include "../http/HTTPRequest.hpp"

void get_peers(std::string&& announce)
{
    http::Request client{announce};

    auto result = client.send()
}