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
          std::vector<std::string>* cluster_labels;
          unsigned int v_num_nodes;
          // Clusters by node
          hmap_uint_suint* node_cluster;
          // For Modularity
          void buildAssortativityMatrix(float** e);
          // For Conductance
          double getClusterPhi(std::set<uint>* clstr);
          double getGraphPhi(std::set<uint>* clstr); // ??
     public:
          ClusterEvaluator();
          ClusterEvaluator(Graph* g);
          ClusterEvaluator(Graph* g, hmap_uint_suint* cls, 
                    hmap_uint_suint* nc, std::vector<std::string>* clbs,
                    uint vnn);
          ~ClusterEvaluator();
          void loadGraph(Graph* g);
          void loadClusters(hmap_uint_suint* cls, hmap_uint_suint* nc);
          // Modularity
          float getModularity();
          // Silhouette
          std::vector<double> getSilhouetteIndex();
          // Entropy
          double getGraphEntropy();
          double getClusterEntropy(uint cid, hmap_s_i* vocw, 
                    uint num_entries_g);
          // Conductance
          double getIntraclusterConductance();
          double getInterclusterConductance();
          // Coverage
          double getCoverage();
          // Single Cluster Editing
          std::vector<double> getSCE();
          // Performance
          double getPerformance();
};
