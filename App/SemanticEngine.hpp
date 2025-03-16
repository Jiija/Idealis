#pragma once

#include "BackendAPIDraft.hpp"
#include <cmath>
#include <memory>
#include <string>
#include <vector>

class SemanticEngine
{
private:
  class SemanticEngineImpl;
  std::unique_ptr<SemanticEngineImpl> pImpl;

public:
  // Constructor - Initializes the LLM model
  SemanticEngine(const std::string& model_path);

  double cosineSimilarity(const Idea& A, const Idea& B);
  double euclideanDistance(const Idea& A, const Idea& B);
  // Destructor - Cleans up resources
  ~SemanticEngine();
};