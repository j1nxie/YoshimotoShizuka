#ifndef TODO_ENTRY_HPP
#define TODO_ENTRY_HPP

#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;

struct TodoEntry {
    int id;
    std::string title;
    bool completed;

    json to_json() const;
};

#endif
