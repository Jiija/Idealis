add_executable(Session Session.cpp)

add_library(SemanticEngine STATIC SemanticEngine.cpp SemanticEngine.hpp)

target_link_libraries(Session PRIVATE DatabaseManager SemanticEngine)

target_link_libraries(SemanticEngine PRIVATE llama)

set(MODEL_SOURCE_PATH "${PROJECT_SOURCE_DIR}/external/models/nomic-embed-text-v1.5.f16.gguf")
set(MODEL_DEST_PATH "${CMAKE_BINARY_DIR}/external/models/")
file(MAKE_DIRECTORY ${MODEL_DEST_PATH})
file(COPY ${MODEL_SOURCE_PATH} DESTINATION ${MODEL_DEST_PATH})
add_custom_target(copy_model ALL DEPENDS ${MODEL_DEST_PATH}/nomic-embed-text-v1.5.f16.gguf)
