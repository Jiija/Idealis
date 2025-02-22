add_library(Session STATIC Session.cpp)

add_library(SemanticEngine STATIC SemanticEngine.cpp)

target_link_libraries(Session PRIVATE DatabaseManager)

target_link_libraries(SemanticEngine PRIVATE llama)
