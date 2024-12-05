#ifndef TODO_SERVER_HPP
#define TODO_SERVER_HPP

#include <asm-generic/socket.h>
#include <netinet/in.h>
#include <sqlite3.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstdint>
#include <string>

#include "YoshimotoShizuka/http/HttpRouter.hpp"

class TodoServer {
private:
    HttpRouter router;
    int server_fd;
    uint16_t port;
    sqlite3 *db;

    void setupRoutes();
    HttpRequest parseRequest(const std::string &request);
    std::string readFullRequest(int socket);
    void handleConnection(int client_socket);

public:
    TodoServer(uint16_t port, sqlite3 *db);
    void start();
    ~TodoServer();
};

#endif
