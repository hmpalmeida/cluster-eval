#include "scan_graph.hpp"
//#include <ext/hash_map>
#include <tr1/unordered_map>
#include <vector>
#include <set>

//typedef __gnu_cxx::hash_map<uint, std::set<uint>* > hmap_uint_suint;
typedef std::tr1::unordered_map<uint, std::set<uint>* > hmap_uint_suint;

class Scan{
     private:
          // Attributes
          std::vector<std::pair<uint, uint> > outliers;
          hmap_uint_suint hubs;
          uint num_clusters;
          //ScanGraph g;
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
          //void buildAssortativityMatrix(float** e);
          double getEdgeWeight(uint node1, uint node2);
          bool similar(uint node1, uint node2, double epsilon);
          double scanSim(uint node1, uint node2);
          double noSelfLoopSim(uint node1, uint node2);
          double weightedMeanSim(uint node1, uint node2);
          double weightedOnlySim(uint node1, uint node2);
          std::string generateFilename(const double epsilon, const int mi);
      public:
          // Will store the nodes' cluster labels (given by scan)
          //  -1   = unclassified
          //   0   = non-member
          // n > 0 = belongs to cluster n
          hmap_ii cluster_label;
          ScanGraph g;
          hmap_uint_suint clusters;
          void run(const double epsilon, const int mi);
          Scan();
          Scan(uint t);
          Scan(uint t, std::string filename);
          void loadGraph(std::string filename);
          ~Scan();
          void setSimFunction(uint simi_type);
          void writeAll(const double epsilon, const int mi);
          void writeClusters(std::string name);
          void writeHubs(std::string name);
          void writeOutliers(std::string name);
          void printGraph();
          //float getModularity();
          long getClusterLabel(uint node);
          void setClusterLabel(uint node, long l);

};
