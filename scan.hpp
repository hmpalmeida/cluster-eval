#include "scan_graph.hpp"
#include <ext/hash_map>
#include <vector>
#include <set>

class Scan{
     private:
          // Attributes
          hmap clusters;
          std::vector<std::pair<uint, uint> > outliers;
          hmap hubs;
          uint num_clusters;
          ScanGraph g;
          // Methods
          bool isCore(uint node, const float epsilon, const int mi);
          float similarity(uint node1, uint node2);
          int getNewClusterID();
          std::set<uint> dirReach(uint v, 
                    const float epsilon, const int mi);
          void addToCluster(uint node, uint clust);
          std::set<uint> isHub(uint node);
          void addHub(uint node, std::set<uint> clusts);
          std::set<uint> neighborhood(uint node, 
                    const float epsilon);
          void buildAssortativityMatrix(float** e);
     public:
          void run(const float epsilon, const int mi);
          Scan();
          Scan(std::string filename);
          void loadGraph(std::string filename);
          ~Scan();
          void printAll();
          void printClusters();
          void printHubs();
          void printGraph();
          void printOutliers();
          float getModularity();
};
