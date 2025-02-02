#pragma once

#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <sqlite3.h>
#include <sstream>

using json = nlohmann::json;

// TODO: do a revision and test

class DatabaseManager
{
private:
  sqlite3* db;

public:
  DatabaseManager(const std::string& dbPath);

  ~DatabaseManager();

  // Function to initialize tables from JSON configuration
  bool initializeTableFromJson(const std::string& jsonFilePath);

  // Function to initialize multiple tables from a folder of JSON files
  void initializeDatabase(const std::vector<std::string>& jsonFiles);

  bool addEntity(const std::string& table,
                 const std::map<std::string, std::string>& values);
  bool updateEntity(const std::string& table,
                    int id,
                    const std::map<std::string, std::string>& values);

  // TODO: refactor functions to use them in addEntity and updateEntity?
  std::optional<std::map<std::string, std::string>> findEntityById(
    const std::string& table,
    int id);
  std::optional<std::map<std::string, std::string>> findEntityByName(
    const std::string& table,
    const std::string& columnName,
    const std::string& name);
};
