#ifndef MIGRATIONS_HPP
#define MIGRATIONS_HPP

#include <sqlite3.h>

#include <filesystem>
#include <string>
#include <vector>

struct Migration {
    int version;
    std::string name;
    std::string sql;
};

class Migrator {
private:
    sqlite3 *db;
    const std::string &migrationsPath;
    std::vector<Migration> migrations;

    int getCurrentVersion();
    void setVersion(int version);
    int executeSql(const std::string &sql);
    void loadMigrations();
    std::string readSqlFile(const std::filesystem::path &path);

public:
    explicit Migrator(sqlite3 *db, const std::string &migrationsPath);
    bool runMigrations();
};

#endif
