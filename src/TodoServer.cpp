#include "YoshimotoShizuka/TodoServer.hpp"

#include <cstdint>
#include <exception>
#include <iostream>
#include <sstream>

#include "YoshimotoShizuka/TodoEntry.hpp"
#include "YoshimotoShizuka/http/HttpRequest.hpp"

TodoServer::TodoServer(uint16_t port, sqlite3 *db) : port(port), db(db) { setupRoutes(); }

HttpRequest TodoServer::parseRequest(const std::string &request) {
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

std::string TodoServer::createHttpResponse(const HttpResponse &response) {
    std::stringstream ss;
    ss << "HTTP/1.1 " << response.status() << " "
       << (response.status() == 200   ? "OK"
           : response.status() == 201 ? "Created"
           : response.status() == 404 ? "Not Found"
                                      : "Bad Request")
       << "\r\n";

    for (const auto &[key, value] : response.headers()) {
        ss << key << ": " << value << "\r\n";
    }

    ss << "Access-Control-Allow-Origin: *\r\n";
    ss << "Content-Length: " << response.body().length() << "\r\n";
    ss << "\r\n";
    ss << response.body();

    return ss.str();
}

void TodoServer::handleConnection(int client_socket) {
    std::string request = readFullRequest(client_socket);
    HttpRequest req = parseRequest(request);
    HttpResponse res;

    try {
        router.handleRequest(req, res);
    } catch (const std::exception &e) {
        res.status(404);
        res.body("Not Found");
    }

    std::string responseStr = createHttpResponse(res);
    send(client_socket, responseStr.c_str(), responseStr.length(), 0);
}

void TodoServer::setupRoutes() {
    router.get("/todos", [this](const HttpRequest &req, HttpResponse &res) {
        json response = json::array();

        sqlite3_stmt *stmt;
        const char *sql = "SELECT * FROM todo;";

        if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
            while (sqlite3_step(stmt) == SQLITE_ROW) {
                int id = sqlite3_column_int(stmt, 0);
                const char *title = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
                bool completed = sqlite3_column_int(stmt, 2);
                const char *created_at =
                    reinterpret_cast<const char *>(sqlite3_column_text(stmt, 3));
                const char *updated_at =
                    reinterpret_cast<const char *>(sqlite3_column_text(stmt, 4));

                TodoEntry todo = {id, title, completed, created_at, updated_at};

                response.push_back(todo.to_json());
            }
        }

        sqlite3_finalize(stmt);

        res.body(response.dump());
    });
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

        handleConnection(new_socket);
        close(new_socket);
    }
}

TodoServer::~TodoServer() {
    close(server_fd);
    sqlite3_close(db);
}
