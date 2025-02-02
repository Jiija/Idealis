#include "DatabaseManager.hpp"

DatabaseManager::DatabaseManager(const std::string& dbPath)
{
  if (sqlite3_open(dbPath.c_str(), &db) != SQLITE_OK)
  {
    std::cerr << "Error opening database: " << sqlite3_errmsg(db) << std::endl;
  }
}

DatabaseManager::~DatabaseManager()
{
  sqlite3_close(db);
}

bool
DatabaseManager::initializeTableFromJson(const std::string& jsonFilePath)
{
  std::ifstream file(jsonFilePath);
  if (!file.is_open())
  {
    std::cerr << "Error: Could not open JSON file: " << jsonFilePath
              << std::endl;
    return false;
  }

  json tableConfig;
  file >> tableConfig;
  file.close();

  std::string tableName = tableConfig["table_name"];
  std::string createTableSQL = "CREATE TABLE IF NOT EXISTS " + tableName + " (";

  for (const auto& column : tableConfig["columns"])
  {
    std::string columnDef = column["name"].get<std::string>() + " " +
                            column["type"].get<std::string>();

    if (column.value("primary_key", false))
    {
      columnDef += " PRIMARY KEY";
    }
    if (column.value("auto_increment", false))
    {
      columnDef += " AUTOINCREMENT";
    }
    if (column.value("not_null", false))
    {
      columnDef += " NOT NULL";
    }
    if (column.contains("default"))
    {
      columnDef += " DEFAULT " + column["default"].get<std::string>();
    }

    createTableSQL += columnDef + ", ";
  }

  // Add foreign key constraints if any
  if (tableConfig.contains("constraints"))
  {
    for (const auto& constraint : tableConfig["constraints"])
    {
      if (constraint["type"] == "FOREIGN KEY")
      {
        createTableSQL +=
          "FOREIGN KEY (" + constraint["columns"][0].get<std::string>() +
          ") REFERENCES " + constraint["references"].get<std::string>() + ", ";
      }
    }
  }

  // Remove the trailing comma and space, then close the statement
  createTableSQL = createTableSQL.substr(0, createTableSQL.size() - 2) + ");";

  // Execute the SQL command
  char* errMsg = nullptr;
  if (sqlite3_exec(db, createTableSQL.c_str(), nullptr, nullptr, &errMsg) !=
      SQLITE_OK)
  {
    std::cerr << "SQL error: " << errMsg << std::endl;
    sqlite3_free(errMsg);
    return false;
  }

  std::cout << "Table '" << tableName << "' initialized successfully!"
            << std::endl;
  return true;
}

// Function to initialize multiple tables from a folder of JSON files
void
DatabaseManager::initializeDatabase(const std::vector<std::string>& jsonFiles)
{
  for (const auto& file : jsonFiles)
  {
    initializeTableFromJson(file);
  }
}

bool
DatabaseManager::addEntity(const std::string& table,
                           const std::map<std::string, std::string>& values)
{
  if (values.empty())
    return false;

  std::string sql = "INSERT INTO " + table + " (";
  std::string placeholders = "(";

  for (const auto& pair : values)
  {
    sql += pair.first + ", ";
    placeholders += "?, ";
  }

  sql.pop_back();
  sql.pop_back(); // Remove trailing ", "
  placeholders.pop_back();
  placeholders.pop_back();
  sql += ") VALUES " + placeholders + ");";

  sqlite3_stmt* stmt;
  if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
  {
    std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db)
              << std::endl;
    return false;
  }

  int i = 1;
  for (const auto& pair : values)
  {
    sqlite3_bind_text(stmt, i++, pair.second.c_str(), -1, SQLITE_STATIC);
  }

  bool success = sqlite3_step(stmt) == SQLITE_DONE;
  sqlite3_finalize(stmt);
  return success;
}

bool
DatabaseManager::updateEntity(const std::string& table,
                              int id,
                              const std::map<std::string, std::string>& values)
{
  if (values.empty())
    return false;

  std::string sql = "UPDATE " + table + " SET ";
  for (const auto& pair : values)
  {
    sql += pair.first + " = ?, ";
  }
  sql.pop_back();
  sql.pop_back();
  sql += " WHERE id = ?;";

  sqlite3_stmt* stmt;
  if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
  {
    std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db)
              << std::endl;
    return false;
  }

  int i = 1;
  for (const auto& pair : values)
  {
    sqlite3_bind_text(stmt, i++, pair.second.c_str(), -1, SQLITE_STATIC);
  }
  sqlite3_bind_int(stmt, i, id);

  bool success = sqlite3_step(stmt) == SQLITE_DONE;
  sqlite3_finalize(stmt);
  return success;
}

std::optional<std::map<std::string, std::string>>
DatabaseManager::findEntityByName(const std::string& table,
                                  const std::string& columnName,
                                  const std::string& name)
{
  std::string sql = "SELECT * FROM " + table + " WHERE " + columnName +
                    " = ? AND is_deleted = FALSE;";
  sqlite3_stmt* stmt;

  if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
  {
    std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db)
              << std::endl;
    return std::nullopt;
  }

  sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);

  std::optional<std::map<std::string, std::string>> result;

  if (sqlite3_step(stmt) == SQLITE_ROW)
  {
    std::map<std::string, std::string> entity;
    int colCount = sqlite3_column_count(stmt);
    for (int i = 0; i < colCount; ++i)
    {
      std::string columnName = sqlite3_column_name(stmt, i);
      std::string value =
        reinterpret_cast<const char*>(sqlite3_column_text(stmt, i));
      entity[columnName] = value;
    }
    result = entity;
  }

  sqlite3_finalize(stmt);
  return result;
}

std::optional<std::map<std::string, std::string>>
DatabaseManager::findEntityById(const std::string& table, int id)
{
  std::string sql =
    "SELECT * FROM " + table + " WHERE id = ? AND is_deleted = FALSE;";
  sqlite3_stmt* stmt;

  if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
  {
    std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db)
              << std::endl;
    return std::nullopt;
  }

  sqlite3_bind_int(stmt, 1, id);

  std::optional<std::map<std::string, std::string>> result;

  if (sqlite3_step(stmt) == SQLITE_ROW)
  {
    std::map<std::string, std::string> entity;
    int colCount = sqlite3_column_count(stmt);
    for (int i = 0; i < colCount; ++i)
    {
      std::string columnName = sqlite3_column_name(stmt, i);
      std::string value =
        reinterpret_cast<const char*>(sqlite3_column_text(stmt, i));
      entity[columnName] = value;
    }
    result = entity;
  }

  sqlite3_finalize(stmt);
  return result;
}
