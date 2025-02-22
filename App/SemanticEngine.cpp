#include "SemanticEngine.hpp"

#include <iostream>
#include <numeric>

// Constructor: Load the LLM model
SemanticEngine::SemanticEngine(const std::string& model_path)
{
  llama_backend_init();
  llama_model_params model_params = llama_model_default_params();
  model = llama_model_load_from_file(model_path.c_str(), model_params);

  if (!model)
  {
    throw std::runtime_error("Failed to load LLM model.");
  }

  // Enable embedding mode
  llama_context_params ctx_params = llama_context_default_params();
  ctx_params.embeddings = true;
  ctx = llama_init_from_model(model, ctx_params);

  if (!ctx)
  {
    throw std::runtime_error("Failed to create LLM context.");
  }

  // Set embedding size based on model (e.g., 768 for nomic-embed-text-v1.5)
  embedding_size = llama_model_n_embd(model);
}

// Destructor: Clean up resources
SemanticEngine::~SemanticEngine()
{
  llama_free(ctx);
  llama_model_free(model);
  llama_backend_free();
}

// Generate embeddings for a given text
std::vector<float>
SemanticEngine::get_embedding(const std::string& text)
{
  // Get the model's vocab (llama.cpp does not expose it directly)
  const llama_vocab* vocab = llama_model_get_vocab(model);
  if (!vocab)
  {
    throw std::runtime_error("Failed to retrieve vocab.");
  }

  // Prepare token buffer
  std::vector<llama_token> tokens(text.length() + 2);

  // Tokenize the input text
  int token_count = llama_tokenize(vocab,
                                   text.c_str(),
                                   text.length(),
                                   tokens.data(),
                                   tokens.size(),
                                   true,
                                   false);
  if (token_count < 0)
  {
    throw std::runtime_error("Tokenization failed.");
  }

  // Resize tokens to actual size
  tokens.resize(token_count);

  // Run inference to get embeddings
  if (llama_decode(ctx, llama_batch_get_one(tokens.data(), tokens.size())))
  {
    throw std::runtime_error("Failed to generate embeddings.");
  }

  // Retrieve the embedding vector
  const float* embedding_data = llama_get_embeddings(ctx);
  return std::vector<float>(embedding_data, embedding_data + embedding_size);
}

// Compute cosine similarity
double
SemanticEngine::cosine_similarity(const std::vector<float>& A,
                                  const std::vector<float>& B)
{
  double dot_product = 0.0, norm_A = 0.0, norm_B = 0.0;

  for (size_t i = 0; i < A.size(); i++)
  {
    dot_product += A[i] * B[i];
    norm_A += A[i] * A[i];
    norm_B += B[i] * B[i];
  }

  return dot_product / (std::sqrt(norm_A) * std::sqrt(norm_B) +
                        1e-9); // Avoid division by zero
}

// Compute normalized Euclidean distance
double
SemanticEngine::normalized_euclidean_distance(const std::vector<float>& A,
                                              const std::vector<float>& B)
{
  double sum = 0.0;
  for (size_t i = 0; i < A.size(); i++)
  {
    sum += (A[i] - B[i]) * (A[i] - B[i]);
  }

  double euclidean = std::sqrt(sum);
  return euclidean / std::sqrt(A.size()); // Normalization factor
}
