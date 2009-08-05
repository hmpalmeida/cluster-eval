#include <ext/hash_map>
#include <vector>
#include <set>
#include "utils.hpp"

typedef __gnu_cxx::hash_map<uint, std::set<uint>*> hmap;

// Class definition. Will probably grow.
class Graph{
   protected:
      uint num_nodes;
      uint num_edges;
      void addEdge(uint node1, uint node2);
   public:
     hmap graph_map;
     std::set<uint>* getAdjacency(uint node);
     void readFile(std::string filename);
     Graph(std::string filename);
     Graph();
     virtual ~Graph();
     void print();
     uint getNumEdges();
};

