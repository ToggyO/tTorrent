#include "http_client.h"

HttpResponse HttpClient::send(const std::string& url,
    const std::string& method ,
    const std::string& body,
    const HttpHeaders& headers,
    std::chrono::milliseconds timeout)
{
    http::Request request{url};
    auto response = request.send("GET");
    return {
        .status = response.status.code,
        .headers = std::move(response.headerFields),
        .body = std::move(response.body)
    };
}