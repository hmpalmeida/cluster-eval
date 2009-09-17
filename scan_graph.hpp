#include "graph.hpp"

// Defining a hash map that maps a uint to a char
typedef __gnu_cxx::hash_map<uint, long> hmap_ii;

// Was in fact created so that we can put the self loops
class ScanGraph: public Graph {
     public:
          // Will store the nodes' labels (given by scan)
          //  -1   = unclassified
          //   0   = non-member
          // n > 0 = belongs to cluster n
          hmap_ii label;
          void readFile(std::string filename, bool self_loops = true);
          void readGmlFile(std::string filename, bool self_loops = true);
          ScanGraph();
          ScanGraph(std::string filename);
          long getLabel(uint node);
          void setLabel(uint node, long l);
};
