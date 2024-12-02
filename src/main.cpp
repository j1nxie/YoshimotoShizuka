#include <asm-generic/socket.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>
#include <nlohmann/json.hpp>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using json = nlohmann::json;

struct TodoEntry {
    int id;
    std::string title;
    bool completed;

    json to_json() const { return {{"id", id}, {"title", title}, {"completed", completed}}; }
};

class TodoServer {
private:
    std::vector<TodoEntry> todos;
    int nextId = 1;
    int server_fd;
    uint16_t port;

    struct HttpRequest {
        std::string method;
        std::string path;
        std::string body;
    };

    HttpRequest parseRequest(const std::string &request) {
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

    std::string createResponse(int status, const std::string &content) {
        std::string status_text = (status == 200)   ? "OK"
                                  : (status == 201) ? "Created"
                                  : (status == 404) ? "Not Found"
                                                    : "Bad Request";

        std::stringstream response;
        response << "HTTP/1.1" << status << " " << status_text << "\r\n";
        response << "Content-Type: application/json\r\n";
        response << "Content-Length: " << content.length() << "\r\n";
        response << "Access-Control-Allow-Origin: *\r\n";
        response << "\r\n";
        response << content;

        return response.str();
    }

    std::string handleGet(const std::string &path) {
        if (path == "/todos") {
            json response = json::array();
            for (const auto &todo : todos) {
                response.push_back(todo.to_json());
            }

            return createResponse(200, response.dump());
        }

        return createResponse(404, "{\"error\": \"Not found\"}");
    }

    std::string handlePost(const std::string &path, const std::string &body) {
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

public:
    TodoServer(uint16_t port) : port(port) {}

    void start() {
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
            char buffer[1024] = {0};

            if ((new_socket = accept(server_fd, reinterpret_cast<struct sockaddr *>(&address),
                                     reinterpret_cast<socklen_t *>(&addrlen))) < 0) {
                throw std::runtime_error("accept failed");
            }

            read(new_socket, buffer, 1024);

            auto request = parseRequest(std::string(buffer));
            std::string response;

            if (request.method == "GET") {
                response = handleGet(request.path);
            } else if (request.method == "POST") {
                response = handlePost(request.path, request.body);
            } else {
                response = createResponse(400, "\"error\": \"Method not supported.\"}");
            }

            send(new_socket, response.c_str(), response.length(), 0);
            close(new_socket);
        }
    }

    ~TodoServer() { close(server_fd); }
};

int main() {
    try {
        TodoServer server(8080);
        server.start();
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
