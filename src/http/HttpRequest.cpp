#include "YoshimotoShizuka/http/HttpRequest.hpp"

#include <string>

std::string HttpRequest::method() const { return _method; }

std::string HttpRequest::path() const { return _path; };

std::string HttpRequest::body() const { return _body; };

void HttpRequest::method(const std::string &method) { _method = method; };

void HttpRequest::path(const std::string &path) { _path = path; };

void HttpRequest::body(const std::string &body) { _body = body; };
