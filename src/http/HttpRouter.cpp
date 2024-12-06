#include "YoshimotoShizuka/http/HttpRouter.hpp"

#include <asm-generic/socket.h>
#include <netinet/in.h>
#include <sqlite3.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>
#include <sstream>
#include <stdexcept>

#include "YoshimotoShizuka/http/HttpRequest.hpp"
#include "YoshimotoShizuka/http/HttpResponse.hpp"

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

HttpRequest HttpRouter::parseRequest(const std::string& request) {
    HttpRequest req;
    std::istringstream iss(request);
    std::string line;

    std::getline(iss, line);
    std::istringstream first_line(line);

    std::string method, path;

    first_line >> method >> path;

    req.method(method);
    req.path(path);

    while (std::getline(iss, line) && line != "\r") {
    }

    std::string body;
    while (std::getline(iss, line)) {
        req.body() += line;
    }

    return req;
}

std::string HttpRouter::readFullRequest(int socket) {
    std::string request;
    std::vector<char> buffer(1024);
    ssize_t bytes_read;

    while ((bytes_read = read(socket, buffer.data(), buffer.size())) > 0) {
        request.append(buffer.data(), bytes_read);

        if (request.find("\r\n\r\n") != std::string::npos) {
            break;
        }
    }

    return request;
}

void HttpRouter::handleRequest(int client_fd) {
    std::string raw_request = readFullRequest(client_fd);
    HttpRequest request = parseRequest(raw_request);
    HttpResponse response;

    try {
        auto handler = findHandler(request.path(), request.method());
        handler(request, response);
    } catch (const std::runtime_error& e) {
        response.status(404);
        response.body("Not Found");
    }

    std::string responseStr = response.toString();
    send(client_fd, responseStr.c_str(), responseStr.length(), 0);
}

void HttpRouter::setupServer() {
    struct sockaddr_in address {};
    int opt = 1;
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_fd < 0) {
        throw std::runtime_error("socket failed");
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        close(server_fd);
        throw std::runtime_error("setsockopt failed");
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, reinterpret_cast<struct sockaddr*>(&address), sizeof(address)) < 0) {
        throw std::runtime_error("bind failed");
    }

    if (::listen(server_fd, 3) < 0) {
        throw std::runtime_error("listen failed");
    }

    ready = true;
}

void HttpRouter::startEventLoop() {
    while (ready) {
        struct sockaddr_in client_addr {};
        socklen_t client_len = sizeof(client_addr);

        int client_fd =
            accept(server_fd, reinterpret_cast<struct sockaddr*>(&client_addr), &client_len);
        if (client_fd < 0) {
            std::cerr << "Failed to accept connection" << std::endl;
            continue;
        }

        try {
            handleRequest(client_fd);
        } catch (const std::exception& e) {
            std::cerr << "Error handling client: " << e.what() << std::endl;
        }

        close(client_fd);
    }
}

HttpRouter::~HttpRouter() {
    if (server_fd >= 0) {
        close(server_fd);
    }
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

void HttpRouter::serve() {
    setupServer();
    startEventLoop();
}
