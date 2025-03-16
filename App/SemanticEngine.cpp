#include "SemanticEngine.hpp"

#include "llama.h" // llama.cpp headers
#include <iostream>
#include <numeric>

class SemanticEngine::SemanticEngineImpl
{
private:
  llama_model* model;
  llama_context* ctx;
  int embedding_size; // Typically 768 for nomic-embed-text-v1.5
public:
  SemanticEngineImpl(const std::string& model_path)
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
    ctx_params.n_ctx = 2048;
    // ctx_params.
    // ctx_params.embedding = true;
    ctx = llama_init_from_model(model, ctx_params);

    if (!ctx)
    {
      throw std::runtime_error("Failed to create LLM context.");
    }

    // Set embedding size based on model (e.g., 768 for nomic-embed-text-v1.5)
    embedding_size = llama_model_n_embd(model);
  }

  ~SemanticEngineImpl()
  {
    llama_free(ctx);
    llama_model_free(model);
    llama_backend_free();
  }

  std::vector<float> getEmbedding(const std::string& text,
                                  const std::string& mode = "clustering")
  {
    // Get the model's vocab (llama.cpp does not expose it directly)
    const llama_vocab* vocab = llama_model_get_vocab(model);
    if (!vocab)
    {
      throw std::runtime_error("Failed to retrieve vocab.");
    }
    std::string prefixed_text = mode + ": " + text;

    int n_tokens = -llama_tokenize(vocab,
                                   prefixed_text.c_str(),
                                   prefixed_text.length(),
                                   NULL,
                                   0,
                                   true,
                                   true);
    if (n_tokens < 0)
    {
      throw std::runtime_error("Tokenization failed.");
    }

    // Step 2: Allocate space and tokenize
    std::vector<llama_token> tokens(n_tokens);
    if (llama_tokenize(vocab,
                       prefixed_text.c_str(),
                       prefixed_text.length(),
                       tokens.data(),
                       tokens.size(),
                       true,
                       true) < 0)
    {
      throw std::runtime_error("Tokenization failed.");
    }
    // Debug: Print tokenized input
    // std::cout << "Tokenized input (" << tokens.size() << " tokens): ";
    // for (auto id : tokens) {
    //    std::cout << id << " ";
    //}
    // std::cout << std::endl;

    llama_batch batch = llama_batch_get_one(tokens.data(), tokens.size());
    // std::cout << "Batch contains " << batch.n_tokens << " tokens." <<
    // std::endl;
    batch.pos = NULL;    // Auto-track token positions
    batch.seq_id = NULL; // Default sequence ID
    // Run inference to get embeddings
    if (llama_model_has_encoder(model) && !llama_model_has_decoder(model))
    {
      // encoder-only model
      if (llama_encode(ctx, batch) < 0)
      {
        throw std::runtime_error("failed to encode");
      }
    }
    else if (!llama_model_has_encoder(model) && llama_model_has_decoder(model))
    {
      // decoder-only model
      if (llama_decode(ctx, batch) < 0)
      {
        throw std::runtime_error("failed to decode");
      }
    }
    // Retrieve the embedding vector
    const float* embedding_data = llama_get_embeddings_seq(ctx, 0);
    return std::vector<float>(embedding_data, embedding_data + embedding_size);
  }

  // Compute cosine similarity
  double cosineSimilarity(const std::vector<float>& A,
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
  double normalizedEuclideanDistance(const std::vector<float>& A,
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
};

SemanticEngine::SemanticEngine(const std::string& model_path)
  : pImpl{ std::make_unique<SemanticEngineImpl>(model_path) }
{
}

SemanticEngine::~SemanticEngine(){};
double
SemanticEngine::cosineSimilarity(const Idea& A, const Idea& B)
{
  return pImpl->cosineSimilarity(pImpl->getEmbedding(A.getContent()),
                                 pImpl->getEmbedding(B.getContent()));
}

double
SemanticEngine::euclideanDistance(const Idea& A, const Idea& B)
{
  return pImpl->normalizedEuclideanDistance(
    pImpl->getEmbedding(A.getContent()), pImpl->getEmbedding(B.getContent()));
}
