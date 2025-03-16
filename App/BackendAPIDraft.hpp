#pragma once

// One side of the intended platform. The other will be about execution
// pipelines that integrate ideas Execution pipeliens should be modular and
// customizable, different workflows etc

// I know this sounds highly abstract, the plan is to have a minimally viable
// product locally and then try to use it for my own purposes to see how well it
// serves its function

// API for program logic. Might (and probably will) be revisited
// Current idea is to generate instances based on a database and then
// save the changes in the database after each session.
// Parsing the entire database and loading it into RAM doesn't sound like a good
// idea, so some kind of database navigation logic will be added

// TODO: do we need an ID system? Can names serve as IDs in the database (I
// think yes)?

#include <string>
#include <vector>

// One instance for one tag; the program is supposed to read from a DB, then
// instantiate tags and link ideas to them when parsing
class Tag
{
  std::string name;
  std::vector<Tag*>
    superTags; // Nested tag system (do we need it?); Maybe will make it
               // bi-directional (having a subTags field as well)
};

// A way to aggregate ideas, as well as focus the attention of the comunity
class Problem
{
private:
  std::string name;
  std::string content;
  std::vector<Tag*>
    tags; // TODO: if tags are nested, I guess we only store tags of the lowest
          // level? Maybe nested tags are redundant?
};

// Three types: strength, weakness, orthogonal (expound on the idea)
// Will probably make subclasses for each type
// Each idea has three tables of respective votes
class Vote
{
  std::string content;
};

class Idea
{
public:
  // Calculates semantic distance between ideas; will be done via semantic
  // embeddings like what @DefenderOfBasic did in his project
  // float semanticDistance(Idea&);
  void pointOutStrenth(Vote*);
  void pointOutWeakness(Vote*);
  void expound(Vote*);
  const std::string getContent() const { return content; }
  void setContent(std::string content_) { content = content_; } // for debug
private:
  std::string name;
  std::string content;
  std::vector<Vote*> strengths;
  std::vector<Vote*> weaknesses;
  std::vector<Vote*> expansions;
  std::vector<Tag*> tags;                // TODO: use smart pointers?
  std::vector<Problem*> relatedProblems; // Problems the idea addresses, if any
};
