#pragma once

#include "llama.h" // llama.cpp headers
#include <cmath>
#include <string>
#include <vector>

class SemanticEngine
{
private:
  llama_model* model;
  llama_context* ctx;
  int embedding_size; // Typically 768 for nomic-embed-text-v1.5

  // Generate embeddings for input text
  std::vector<float> get_embedding(const std::string& text);
  // Compute cosine similarity between two embeddings
  static double cosine_similarity(const std::vector<float>& A,
                                  const std::vector<float>& B);

  // Compute normalized Euclidean distance between two embeddings
  static double normalized_euclidean_distance(const std::vector<float>& A,
                                              const std::vector<float>& B);

public:
  // Constructor - Initializes the LLM model
  SemanticEngine(const std::string& model_path);

  // Destructor - Cleans up resources
  ~SemanticEngine();
};