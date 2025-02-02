#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <sqlite3.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

//TODO: do a revision and test

class DatabaseManager
{
private:
    sqlite3* db;

public:
    DatabaseManager(const std::string& dbPath) {
        if (sqlite3_open(dbPath.c_str(), &db) != SQLITE_OK) {
            std::cerr << "Error opening database: " << sqlite3_errmsg(db) << std::endl;
        }
    }

    ~DatabaseManager() {
        sqlite3_close(db);
    }

    // Function to initialize tables from JSON configuration
    bool initializeTableFromJson(const std::string& jsonFilePath) {
        std::ifstream file(jsonFilePath);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open JSON file: " << jsonFilePath << std::endl;
            return false;
        }

        json tableConfig;
        file >> tableConfig;
        file.close();

        std::string tableName = tableConfig["table_name"];
        std::string createTableSQL = "CREATE TABLE IF NOT EXISTS " + tableName + " (";

        for (const auto& column : tableConfig["columns"]) {
            std::string columnDef = column["name"].get<std::string>() + " " + column["type"].get<std::string>();

            if (column.value("primary_key", false)) {
                columnDef += " PRIMARY KEY";
            }
            if (column.value("auto_increment", false)) {
                columnDef += " AUTOINCREMENT";
            }
            if (column.value("not_null", false)) {
                columnDef += " NOT NULL";
            }
            if (column.contains("default")) {
                columnDef += " DEFAULT " + column["default"].get<std::string>();
            }

            createTableSQL += columnDef + ", ";
        }

        // Add foreign key constraints if any
        if (tableConfig.contains("constraints")) {
            for (const auto& constraint : tableConfig["constraints"]) {
                if (constraint["type"] == "FOREIGN KEY") {
                    createTableSQL += "FOREIGN KEY (" + constraint["columns"][0].get<std::string>() + ") REFERENCES " +
                                      constraint["references"].get<std::string>() + ", ";
                }
            }
        }

        // Remove the trailing comma and space, then close the statement
        createTableSQL = createTableSQL.substr(0, createTableSQL.size() - 2) + ");";

        // Execute the SQL command
        char* errMsg = nullptr;
        if (sqlite3_exec(db, createTableSQL.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
            std::cerr << "SQL error: " << errMsg << std::endl;
            sqlite3_free(errMsg);
            return false;
        }

        std::cout << "Table '" << tableName << "' initialized successfully!" << std::endl;
        return true;
    }

    // Function to initialize multiple tables from a folder of JSON files
    void initializeDatabase(const std::vector<std::string>& jsonFiles) {
        for (const auto& file : jsonFiles) {
            initializeTableFromJson(file);
        }
    }
};