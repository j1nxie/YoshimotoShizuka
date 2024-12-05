#include "YoshimotoShizuka/db/Migrator.hpp"

#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <stdexcept>

Migrator::Migrator(sqlite3 *db, const std::string &migrationsPath)
    : db(db), migrationsPath(migrationsPath) {
    loadMigrations();
}

int Migrator::getCurrentVersion() {
    sqlite3_stmt *stmt;
    const char *sql = "PRAGMA user_version;";
    int version = 0;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            version = sqlite3_column_int(stmt, 0);
        }
    }

    sqlite3_finalize(stmt);

    return version;
}

void Migrator::setVersion(int version) {
    std::string sql = "PRAGMA user_version = " + std::to_string(version) + ";";

    executeSql(sql);
}

int Migrator::executeSql(const std::string &sql) {
    char *errMsg = nullptr;
    int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);

    if (rc != SQLITE_OK) {
        std::string error = errMsg;
        sqlite3_free(errMsg);
        throw std::runtime_error("SQL error: " + error);
    }

    return rc;
}

void Migrator::loadMigrations() {
    const std::regex migrationFileRegex(R"((\d{6})_([^.]+)\.sql)");

    for (const auto &entry : std::filesystem::directory_iterator(migrationsPath)) {
        if (entry.path().extension() == ".sql") {
            std::string filename = entry.path().filename().string();
            std::smatch matches;

            if (std::regex_match(filename, matches, migrationFileRegex)) {
                int version = std::stoi(matches[1]);
                std::string name = matches[2];
                std::string sql = readSqlFile(entry.path());

                migrations.push_back({version, name, sql});
            }
        }
    }

    std::sort(migrations.begin(), migrations.end(),
              [](const Migration &a, const Migration &b) { return a.version < b.version; });
}

std::string Migrator::readSqlFile(const std::filesystem::path &path) {
    std::ifstream file(path);

    if (!file.is_open()) {
        throw std::runtime_error("failed to open file: " + path.string());
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    return buffer.str();
}

bool Migrator::runMigrations() {
    int currentVersion = getCurrentVersion();

    executeSql("BEGIN TRANSACTION;");

    try {
        for (const auto &migration : migrations) {
            if (migration.version > currentVersion) {
                std::cout << "applying migration: " << migration.version << " - " << migration.name
                          << std::endl;

                if (executeSql(migration.sql) != SQLITE_OK) {
                    throw std::runtime_error("migration failed.");
                }

                setVersion(migration.version);
            }
        }

        executeSql("COMMIT;");
    } catch (const std::exception &e) {
        executeSql("ROLLBACK;");
        std::cerr << "migration failed: " << e.what() << std::endl;

        return false;
    }

    return true;
}
