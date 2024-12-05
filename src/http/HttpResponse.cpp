#include "YoshimotoShizuka/http/HttpResponse.hpp"

#include <sstream>
#include <string>

int HttpResponse::status() const { return _status; };

std::string HttpResponse::body() const { return _body; };

std::unordered_map<std::string, std::string> HttpResponse::headers() const { return _headers; }

void HttpResponse::body(const std::string& content) { _body = content; }

void HttpResponse::status(int code) { _status = code; }

std::string HttpResponse::toString() const {
    std::stringstream ss;

    switch (_status) {
        case 200:
            ss << "HTTP/1.1 200 OK\r\n";
            break;
        case 201:
            ss << "HTTP/1.1 201 Created\r\n";
            break;
        case 404:
            ss << "HTTP/1.1 404 Not Found\r\n";
            break;
        default:
            ss << "HTTP/1.1 400 Bad Request\r\n";
            break;
    }

    for (const auto& [key, value] : _headers) {
        ss << key << ": " << value << "\r\n";
    }

    ss << "Access-Control-Allow-Origin: *\r\n";
    ss << "Content-Length: " << _body.length() << "\r\n";
    ss << "\r\n";
    ss << _body;

    return ss.str();
}
