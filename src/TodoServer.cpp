#include "YoshimotoShizuka/TodoServer.hpp"

#include "YoshimotoShizuka/db/TodoRepository.hpp"
#include "YoshimotoShizuka/http/HttpRequest.hpp"
#include "YoshimotoShizuka/http/HttpResponse.hpp"
#include "YoshimotoShizuka/models/TodoEntry.hpp"

void TodoServer::handleGetTodos(const HttpRequest &_, HttpResponse &res) {
    auto todos = repo.findAll();

    json response = json::array();

    for (const auto &todo : todos) {
        response.push_back(todo.to_json());
    }

    res.body(response.dump());
}

void TodoServer::setupRoutes(){
    router.get("/todos",
               [this](const HttpRequest &req, HttpResponse &res) { handleGetTodos(req, res); })}

TodoServer::TodoServer(TodoRepository repo)
    : repo(repo) {
    setupRoutes();
}

void TodoServer::start() { router.serve(); }
