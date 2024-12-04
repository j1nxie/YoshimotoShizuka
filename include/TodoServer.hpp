#ifndef TODO_SERVER_HPP
#define TODO_SERVER_HPP

#include <asm-generic/socket.h>
#include <netinet/in.h>
#include <sqlite3.h>
#include <sys/socket.h>
#include <unistd.h>

#include <TodoEntry.hpp>
#include <cstdint>
#include <nlohmann/json.hpp>
#include <string>

#include "TodoEntry.hpp"

using json = nlohmann::json;

class TodoServer {
private:
    std::vector<TodoEntry> todos;
    int nextId = 1;
    int server_fd;
    uint16_t port;
    sqlite3 *db;

    struct HttpRequest {
        std::string method;
        std::string path;
        std::string body;
    };

    HttpRequest parseRequest(const std::string &request);
    std::string createResponse(int status, const std::string &content);
    std::string handleGet(const std::string &path);
    std::string handlePost(const std::string &path, const std::string &body);

public:
    TodoServer(uint16_t port, sqlite3 *db);
    void start();
    ~TodoServer();
};

#endif
