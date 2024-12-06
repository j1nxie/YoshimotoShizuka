#ifndef HTTP_ROUTER_HPP
#define HTTP_ROUTER_HPP

#include <functional>
#include <string>
#include <unordered_map>
#include <variant>

#include "HttpRequest.hpp"
#include "HttpResponse.hpp"

enum class StandardHttpMethod { GET, POST, PUT, DELETE, PATCH, HEAD, OPTIONS };

using HttpMethod = std::variant<StandardHttpMethod, std::string>;

using HandlerFunction = std::function<void(const HttpRequest&, HttpResponse&)>;

class HttpRoute {
private:
    std::string path;
    HttpMethod method;
    HandlerFunction handler;

public:
    HttpRoute(const std::string& p, HttpMethod m, HandlerFunction h);
};

class HttpRouter {
private:
    std::unordered_map<std::string, std::unordered_map<HttpMethod, HandlerFunction>> routes;

    std::string methodToString(const HttpMethod& method) const;

public:
    HttpRouter& handle(const std::string& path, HttpMethod method, HandlerFunction handler);

    HttpRouter& get(const std::string& path, HandlerFunction handler);

    HttpRouter& post(const std::string& path, HandlerFunction handler);

    HttpRouter& put(const std::string& path, HandlerFunction handler);

    HttpRouter& del(const std::string& path, HandlerFunction handler);

    HttpRouter& patch(const std::string& path, HandlerFunction handler);

    HttpRouter& head(const std::string& path, HandlerFunction handler);

    HandlerFunction findHandler(const std::string& path, const std::string& method) const;

    void handleRequest(const HttpRequest& req, HttpResponse& res);
};

#endif
