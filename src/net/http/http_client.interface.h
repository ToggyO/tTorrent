#pragma once

#include <chrono>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

using HttpHeader = std::pair<std::string, std::string>;
using HttpHeaders = std::vector<HttpHeader>;

struct HttpResponse
{
    std::uint16_t status;
    HttpHeaders headers;
    std::vector<std::uint8_t> body;
};

// TODO: add descr
struct IHttpClient
{
    virtual HttpResponse send(const std::string& url,
          const std::string& method = "GET",
          const std::string& body = "",
          const HttpHeaders& headers = {},
          std::chrono::milliseconds timeout = std::chrono::milliseconds{-1}) = 0;
};