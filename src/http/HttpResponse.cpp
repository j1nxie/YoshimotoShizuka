#include "YoshimotoShizuka/http/HttpResponse.hpp"

#include <string>

int HttpResponse::status() const { return _status; };

std::string HttpResponse::body() const { return _body; };

std::unordered_map<std::string, std::string> HttpResponse::headers() const { return _headers; }

void HttpResponse::body(const std::string& content) { _body = content; }

void HttpResponse::status(int code) { _status = code; }
