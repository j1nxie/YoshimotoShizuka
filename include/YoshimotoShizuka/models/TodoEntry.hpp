#ifndef TODO_ENTRY_HPP
#define TODO_ENTRY_HPP

#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;

struct TodoEntry {
    int id;
    std::string title;
    bool completed;
    std::string created_at;
    std::string updated_at;

    json to_json() const;
};

#endif
