#include "graph.hpp"
#include <tr1/unordered_map>
#include <set>
#include <vector>

typedef std::tr1::unordered_map<uint, std::set<uint>* > hmap_uint_suint;
typedef std::tr1::unordered_map<uint, long> hmap_ii;

class ClusterEvaluator {
     private:
          unsigned int num_clusters;
          Graph* graph;
          hmap_uint_suint* clusters;
          // Clusters by node
          hmap_uint_suint* node_cluster;
          void buildAssortativityMatrix(float** e);
     public:
          ClusterEvaluator();
          ClusterEvaluator(Graph* g);
          ClusterEvaluator(Graph* g, hmap_uint_suint* cls, hmap_uint_suint* nc);
          ~ClusterEvaluator();
          void loadGraph(Graph* g);
          void loadClusters(hmap_uint_suint* cls, hmap_uint_suint* nc);
          float getModularity();
          std::vector<double> getSilhouetteIndex();
          //std::vector<double> getSilhouetteIndex(hmap_uint_suint* nc);
};
