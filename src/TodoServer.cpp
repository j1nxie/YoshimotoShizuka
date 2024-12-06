#include "YoshimotoShizuka/TodoServer.hpp"

#include "YoshimotoShizuka/http/HttpRequest.hpp"
#include "YoshimotoShizuka/models/TodoEntry.hpp"

void TodoServer::setupRoutes() {
    router.get("/todos", [this](const HttpRequest &_, HttpResponse &res) {
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

TodoServer::TodoServer(sqlite3 *db) : db(db) { setupRoutes(); }

void TodoServer::start() { router.serve(); }

TodoServer::~TodoServer() { sqlite3_close(db); }
