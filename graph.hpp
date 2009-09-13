#include <ext/hash_map>
#include <vector>
#include <set>
#include "edge.hpp"

typedef __gnu_cxx::hash_map<uint, std::set<Edge>*> hmap;

// Class definition. Will probably grow.
class Graph{
     protected:
          uint num_nodes;
          uint num_edges;
          double greatest_weight;
          double smallest_weight;
          void addEdge(uint node1, uint node2, double weight = 0.0);
          void normalizeWeights();
     public:
          hmap graph_map;
          std::set<Edge>* getAdjacency(uint node);
          void readFile(std::string filename);
          void readGmlFile(std::string filename);
          Graph(std::string filename);
          Graph();
          virtual ~Graph();
          void print();
          uint getNumEdges();
};

