add_library(Session STATIC Session.cpp)

target_link_libraries(Session PRIVATE DatabaseManager)