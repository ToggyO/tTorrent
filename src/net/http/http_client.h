#pragma once

#include "http_client.interface.h"
#include "../../../lib/net/http/HTTPRequest.hpp"

// TODO: add descr
struct HttpClient : public IHttpClient
{
    HttpClient() = default;

    HttpResponse send(const std::string& url,
        const std::string& method = "GET",
        const std::string& body = "",
        const HttpHeaders& headers = {},
        std::chrono::milliseconds timeout = std::chrono::milliseconds{-1}) final;
};
