find_package(SQLite3 REQUIRED)

add_library(DatabaseManager STATIC DatabaseManager.cpp)

target_include_directories(DatabaseManager PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(DatabaseManager PRIVATE /usr/include/nlohmann)

target_link_libraries(DatabaseManager PRIVATE SQLite::SQLite3)