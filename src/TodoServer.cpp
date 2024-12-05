#include "YoshimotoShizuka/TodoServer.hpp"

#include <cstdint>
#include <iostream>
#include <sstream>

TodoServer::TodoServer(uint16_t port, sqlite3 *db) : port(port), db(db) {}

TodoServer::HttpRequest TodoServer::parseRequest(const std::string &request) {
    HttpRequest req;
    std::istringstream iss(request);
    std::string line;

    std::getline(iss, line);
    std::istringstream first_line(line);

    first_line >> req.method >> req.path;

    while (std::getline(iss, line) && line != "\r") {
    }

    std::string body;
    while (std::getline(iss, line)) {
        req.body += line;
    }

    return req;
}

std::string TodoServer::readFullRequest(int socket) {
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

std::string TodoServer::handleConnection(int client_socket) {
    std::string request = readFullRequest(client_socket);
    auto parsed_request = parseRequest(request);

    if (parsed_request.method == "GET") {
        return handleGet(parsed_request.path);
    } else if (parsed_request.method == "POST") {
        return handlePost(parsed_request.path, parsed_request.body);
    } else {
        return createResponse(400, "{\"error\": \"Method not supported.\"}");
    }
}

std::string TodoServer::createResponse(int status, const std::string &content) {
    std::string status_text = (status == 200)   ? "OK"
                              : (status == 201) ? "Created"
                              : (status == 404) ? "Not Found"
                                                : "Bad Request";

    std::stringstream response;
    response << "HTTP/1.1 " << status << " " << status_text << "\r\n";
    response << "Content-Type: application/json\r\n";
    response << "Content-Length: " << content.length() << "\r\n";
    response << "Access-Control-Allow-Origin: *\r\n";
    response << "\r\n";
    response << content;

    return response.str();
}

std::string TodoServer::handleGet(const std::string &path) {
    if (path == "/todos") {
        json response = json::array();
        for (const auto &todo : todos) {
            response.push_back(todo.to_json());
        }

        return createResponse(200, response.dump());
    }

    return createResponse(404, "{\"error\": \"Not found\"}");
}

std::string TodoServer::handlePost(const std::string &path, const std::string &body) {
    if (path == "/todos") {
        try {
            auto json_body = json::parse(body);
            TodoEntry newTodo{nextId++, json_body["title"], false};
            todos.push_back(newTodo);

            return createResponse(201, newTodo.to_json().dump());
        } catch (const std::exception &e) {
            return createResponse(400, "{\"error\": \"Invalid request\"}");
        }
    }

    return createResponse(400, "{\"error\": \"Invalid request\"}");
}

void TodoServer::start() {
    struct sockaddr_in address;
    int opt = 1;

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        throw std::runtime_error("socket failed");
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        throw std::runtime_error("setsockopt failed");
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, reinterpret_cast<struct sockaddr *>(&address), sizeof(address)) < 0) {
        throw std::runtime_error("bind failed");
    }

    if (listen(server_fd, 3) < 0) {
        throw std::runtime_error("listen failed");
    }

    std::cout << "Yoshimoto Shizuka listening on port " << port << std::endl;

    while (true) {
        int new_socket;
        int addrlen = sizeof(address);

        if ((new_socket = accept(server_fd, reinterpret_cast<struct sockaddr *>(&address),
                                 reinterpret_cast<socklen_t *>(&addrlen))) < 0) {
            throw std::runtime_error("accept failed");
        }

        std::string response = handleConnection(new_socket);

        send(new_socket, response.c_str(), response.length(), 0);
        close(new_socket);
    }
}

TodoServer::~TodoServer() {
    close(server_fd);
    sqlite3_close(db);
}
