#ifndef TODO_SERVER_HPP
#define TODO_SERVER_HPP

#include <asm-generic/socket.h>
#include <netinet/in.h>
#include <sqlite3.h>
#include <sys/socket.h>
#include <unistd.h>

#include "YoshimotoShizuka/http/HttpRouter.hpp"

class TodoServer {
private:
    HttpRouter router;
    sqlite3 *db;

    void setupRoutes();

public:
    TodoServer(sqlite3 *db);
    void start();
    ~TodoServer();
};

#endif
