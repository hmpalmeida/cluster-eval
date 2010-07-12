//#include <ext/hash_map>
#include <tr1/unordered_map>
#include <vector>
#include <set>
#include "edge.hpp"
#include <string>

//typedef __gnu_cxx::hash_map< uint, std::set<Edge>*> hmap;
//typedef __gnu_cxx::hash_map< uint, std::set<std::string>*> hmap_i_ss;
//typedef __gnu_cxx::hash_map< uint, std::string > hmap_i_s;

#ifndef CLUSTERING_GRAPH_H_
#define CLUSTERING_GRAPH_H_

#define MAXIMUM 999999999

typedef std::tr1::unordered_map< uint, std::set<Edge>*> hmap;
typedef std::tr1::unordered_map< uint, std::set<std::string>*> hmap_i_ss;
typedef std::tr1::unordered_map< uint, std::string > hmap_i_s;
typedef std::tr1::unordered_map< std::string, uint > hmap_s_i;
typedef std::tr1::unordered_map< uint, uint > hmap_i_i;

// Class definition. Will probably grow.
class Graph{
     protected:
          bool directed;
          uint num_nodes;
          uint num_edges;
          double greatest_weight;
          double smallest_weight;
          void addEdge(uint node1, uint node2, double weight = 1.0);
          void normalizeWeights();
          hmap_i_s graph_labels;
          hmap_s_i label_id;
     public:
          hmap graph_map;
          // Gambiarra? sim, mas...
          hmap_i_ss graph_attributes;
          std::set<Edge>* getAdjacency(uint node);
          void readFile(std::string filename);
          void readGmlFile(std::string filename);
          Graph(std::string filename);
          Graph();
          virtual ~Graph();
          void print();
          void printDotFile(std::string filename);
          void printWeights();
          uint getNumEdges();
          uint getNumNodes();
          std::set<std::string>* getVocabulary(uint node);
          bool hasWord(uint node, std::string word);
          void addWord(uint node, std::string word);
          void dropVocabulary(uint node);
          void printVocabularies();
          std::string getNodeLabel(uint node);
          void printDegreeDistroData();
          void printOtterFile(std::string filename);
          void printClutoFile(std::string filename);
          bool isDirected();
          hmap_i_i dijkstra(uint source);
          uint getNodeLabelId(std::string label);
};

#endif  // CLUSTERING_GRAPH_H_
