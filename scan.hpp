#include "scan_graph.hpp"
#include <ext/hash_map>
#include <vector>
#include <set>

typedef __gnu_cxx::hash_map<uint, std::set<uint>* > hmap_uint_suint;

class Scan{
     private:
          // Attributes
          hmap_uint_suint clusters;
          std::vector<std::pair<uint, uint> > outliers;
          hmap_uint_suint hubs;
          uint num_clusters;
          ScanGraph g;
          // Methods
          bool isCore(uint node, const float epsilon, const int mi);
          float similarity(uint node1, uint node2);
          int getNewClusterID();
          std::set<Edge> dirReach(uint v, 
                    const float epsilon, const int mi);
          void addToCluster(uint node, uint clust);
          std::set<uint> getNeighborClusters(uint node);
          void addHub(uint node, std::set<uint> clusts);
          std::set<Edge> neighborhood(uint node, 
                    const float epsilon);
          void buildAssortativityMatrix(float** e);
     public:
          void run(const float epsilon, const int mi);
          Scan();
          Scan(std::string filename);
          void loadGraph(std::string filename);
          ~Scan();
          void writeAll();
          void writeClusters();
          void writeHubs();
          void printGraph();
          void writeOutliers();
          float getModularity();
};
