#include "YoshimotoShizuka/db/TodoRepository.hpp"

#include <optional>

#include "YoshimotoShizuka/models/TodoEntry.hpp"

TodoRepository::TodoRepository(sqlite3 *db) : db(db) {}

std::vector<TodoEntry> TodoRepository::findAll() {
    std::vector<TodoEntry> todos;
    sqlite3_stmt *stmt;
    const char *sql = "SELECT * FROM todo;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            const char *title = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
            bool completed = sqlite3_column_int(stmt, 2);
            const char *created_at = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 3));
            const char *updated_at = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 4));

            todos.emplace_back(id, title, completed, created_at, updated_at);
        }
    }

    sqlite3_finalize(stmt);
    return todos;
}

std::optional<TodoEntry> TodoRepository::findById(int id) {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT * from todo WHERE id = ?";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, id);

        if (sqlite3_step(stmt) == SQLITE_ROW) {
            int entry_id = sqlite3_column_int(stmt, 0);
            const char *title = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
            bool completed = sqlite3_column_int(stmt, 2);
            const char *created_at = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 3));
            const char *updated_at = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 4));

            sqlite3_finalize(stmt);

            return TodoEntry{entry_id, title, completed, created_at, updated_at};
        }
    }

    sqlite3_finalize(stmt);

    return std::nullopt;
}

TodoEntry TodoRepository::create(const std::string &title) {
    // TODO
}

bool TodoRepository::update(const TodoEntry &todo) {
    // TODO
}

bool TodoRepository::delete_(int id) {
    // TODO
}
