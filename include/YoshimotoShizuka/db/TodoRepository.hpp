#ifndef TODO_REPOSITORY_HPP
#define TODO_REPOSITORY_HPP

#include <sqlite3.h>

#include <optional>

#include "YoshimotoShizuka/models/TodoEntry.hpp"

class TodoRepository {
private:
    sqlite3* db;

public:
    explicit TodoRepository(sqlite3* db);

    std::vector<TodoEntry> findAll();
    std::optional<TodoEntry> findById(int id);
    TodoEntry create(const std::string& title);
    bool update(const TodoEntry& todo);
    bool delete_(int id);
};

#endif
