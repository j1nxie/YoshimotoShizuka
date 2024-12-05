#include <asm-generic/socket.h>
#include <netinet/in.h>
#include <sqlite3.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>
#include <nlohmann/json.hpp>

#include "YoshimotoShizuka/TodoServer.hpp"
#include "YoshimotoShizuka/db/Migrator.hpp"

int main() {
    try {
        sqlite3 *db;
        int result = sqlite3_open("database.db", &db);

        if (result != SQLITE_OK) {
            std::cerr << "error opening db: " << sqlite3_errmsg(db) << std::endl;
            return 1;
        }

        Migrator migrator(db, "migrations");

        if (!migrator.runMigrations()) {
            std::cerr << "failed to run database migrations." << std::endl;
            sqlite3_close(db);

            return 1;
        }

        TodoServer server(8080, db);

        server.start();
    } catch (const std::exception &e) {
        std::cerr << "error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
