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

HttpRouter& HttpRouter::handle(const std::string& path, HttpMethod method,
                               HandlerFunction handler) {
    routes[path][methodToString(method)] = handler;
    return *this;
}

HttpRouter& HttpRouter::get(const std::string& path, HandlerFunction handler) {
    return handle(path, StandardHttpMethod::GET, std::move(handler));
}

HttpRouter& HttpRouter::post(const std::string& path, HandlerFunction handler) {
    return handle(path, StandardHttpMethod::POST, std::move(handler));
}

HttpRouter& HttpRouter::put(const std::string& path, HandlerFunction handler) {
    return handle(path, StandardHttpMethod::PUT, std::move(handler));
}

HttpRouter& HttpRouter::del(const std::string& path, HandlerFunction handler) {
    return handle(path, StandardHttpMethod::DELETE, std::move(handler));
}

HttpRouter& HttpRouter::patch(const std::string& path, HandlerFunction handler) {
    return handle(path, StandardHttpMethod::PATCH, std::move(handler));
}

HttpRouter& HttpRouter::head(const std::string& path, HandlerFunction handler) {
    return handle(path, StandardHttpMethod::HEAD, std::move(handler));
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
