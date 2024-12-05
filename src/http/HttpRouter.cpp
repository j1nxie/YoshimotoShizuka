#include "YoshimotoShizuka/http/HttpRouter.hpp"

#include <stdexcept>

HttpRoute::HttpRoute(const std::string& p, HttpMethod m, HandlerFunction h)
    : path(p), method(m), handler(h) {}

std::string HttpRouter::methodToString(const HttpMethod& method) const {
    if (auto* stdMethod = std::get_if<StandardHttpMethod>(&method)) {
        switch (*stdMethod) {
            case StandardHttpMethod::GET:
                return "GET";
            case StandardHttpMethod::POST:
                return "POST";
            case StandardHttpMethod::PUT:
                return "PUT";
            case StandardHttpMethod::DELETE:
                return "DELETE";
            case StandardHttpMethod::PATCH:
                return "PATCH";
            case StandardHttpMethod::HEAD:
                return "HEAD";
            case StandardHttpMethod::OPTIONS:
                return "OPTIONS";
        }
    }

    return std::get<std::string>(method);
}

void HttpRouter::handle(const std::string& path, HttpMethod method, HandlerFunction handler) {
    routes[path][methodToString(method)] = handler;
}

void HttpRouter::get(const std::string& path, HandlerFunction handler) {
    handle(path, StandardHttpMethod::GET, handler);
}

void HttpRouter::post(const std::string& path, HandlerFunction handler) {
    handle(path, StandardHttpMethod::POST, handler);
}

void HttpRouter::put(const std::string& path, HandlerFunction handler) {
    handle(path, StandardHttpMethod::PUT, handler);
}

void HttpRouter::del(const std::string& path, HandlerFunction handler) {
    handle(path, StandardHttpMethod::DELETE, handler);
}

void HttpRouter::patch(const std::string& path, HandlerFunction handler) {
    handle(path, StandardHttpMethod::PATCH, handler);
}

void HttpRouter::head(const std::string& path, HandlerFunction handler) {
    handle(path, StandardHttpMethod::HEAD, handler);
}

HandlerFunction HttpRouter::findHandler(const std::string& path, const std::string& method) const {
    auto pathIt = routes.find(path);
    if (pathIt == routes.end()) {
        throw std::runtime_error("path not found.");
    }

    auto methodIt = pathIt->second.find(method);
    if (methodIt == pathIt->second.end()) {
        throw std::runtime_error("method not allowed.");
    }

    return methodIt->second;
}

void HttpRouter::handleRequest(const HttpRequest& req, HttpResponse& res) {
    try {
        auto handler = HttpRouter::findHandler(req.path(), req.method());
        handler(req, res);
    } catch (const std::runtime_error& e) {
        res.status(404);
        res.body("Not Found");
    }
}
