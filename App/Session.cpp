#include "DatabaseManager.hpp"
#include "SemanticEngine.hpp"

#include <iostream>

class Session
{
private:
  DatabaseManager dbManager;
};

int
main()
{
  Idea A, B, C, D;
  SemanticEngine engine("../external/models/nomic-embed-text-v1.5.f16.gguf");
  A.setContent("Vertically integrated modular manufacturing of specialized "
               "manufacturing tools.");
  B.setContent("Platform for sharing and evaluating ideas, with mechanism for "
               "organizing the implementation and drawing funds.");
  C.setContent(
    "Task description and backlog designed in a way that any member of the "
    "team can freely pick up the task off the top of the backlog");
  D.setContent("Isolated domes with different GMO ecosystem for purposes such "
               "as research or agriculture");
  std::cout << "Cosine A B: " << engine.cosineSimilarity(A, B)
            << " Euclidean A B: " << engine.euclideanDistance(A, B) << std::endl
            << "Cosine A C: " << engine.cosineSimilarity(A, C)
            << " Euclidean A C: " << engine.euclideanDistance(A, C) << std::endl
            << "Cosine B C: " << engine.cosineSimilarity(B, C)
            << " Euclidean B C: " << engine.euclideanDistance(B, C) << std::endl
            << "Cosine C D: " << engine.cosineSimilarity(C, D)
            << " Euclidean C D: " << engine.euclideanDistance(C, D)
            << std::endl;
}
