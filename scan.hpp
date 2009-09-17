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
          uint type; // Which similarity function to use
          // Methods
          bool isCore(uint node, const double epsilon, const int mi);
          int getNewClusterID();
          std::set<Edge> dirReach(uint v, 
                    const double epsilon, const int mi);
          void addToCluster(uint node, uint clust);
          std::set<uint> getNeighborClusters(uint node);
          void addHub(uint node, std::set<uint> clusts);
          std::set<Edge> neighborhood(uint node, 
                    const double epsilon);
          void buildAssortativityMatrix(float** e);
          double similarity(uint node1, uint node2);
          double scanSim(uint node1, uint node2);
          double noSelfLoopSim(uint node1, uint node2);
          double weightedSim(uint node1, uint node2);
     public:
          void run(const double epsilon, const int mi);
          Scan();
          Scan(uint t);
          Scan(uint t, std::string filename);
          void loadGraph(std::string filename);
          ~Scan();
          void setSimFunction(uint simi_type);
          void writeAll();
          void writeClusters();
          void writeHubs();
          void printGraph();
          void writeOutliers();
          float getModularity();
};
