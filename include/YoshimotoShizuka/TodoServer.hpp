#ifndef TODO_SERVER_HPP
#define TODO_SERVER_HPP

#include <asm-generic/socket.h>
#include <netinet/in.h>
#include <sqlite3.h>
#include <sys/socket.h>
#include <unistd.h>

#include "YoshimotoShizuka/db/TodoRepository.hpp"
#include "YoshimotoShizuka/http/HttpRequest.hpp"
#include "YoshimotoShizuka/http/HttpRouter.hpp"

class TodoServer {
private:
    HttpRouter router;
    TodoRepository repo;

    void setupRoutes();
    void handleGetTodos(const HttpRequest& req, HttpResponse& res);

public:
    TodoServer(TodoRepository repo);
    void start();
    ~TodoServer();
};

#endif
