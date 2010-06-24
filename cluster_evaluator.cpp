#include "utils.hpp"
#include "cluster_evaluator.hpp"

/********************************************************************
* Modularity Stuff
* FIXME Modularidade vai ter que virar uma classe em si para eu poder
* usar com o naming game!
********************************************************************/

ClusterEvaluator::ClusterEvaluator() {
}

ClusterEvaluator::ClusterEvaluator(Graph* g) {
     loadGraph(g);
}

ClusterEvaluator::ClusterEvaluator(Graph* g, hmap_uint_suint* cls, 
          hmap_ii* nc) {
     loadGraph(g);
     loadClusters(cls, nc);
}

ClusterEvaluator::~ClusterEvaluator() {
}

void ClusterEvaluator::loadGraph(Graph* g) {
     graph = g;
}

void ClusterEvaluator::loadClusters(hmap_uint_suint* cls, hmap_ii* nc) {
     clusters = cls;
     num_clusters = clusters->size();
     node_cluster = nc;
}


/********************************************************************
* Builds the assortativity marix used in modularity calculations
********************************************************************/
void ClusterEvaluator::buildAssortativityMatrix(float** e) {
     // First, let's "zero" it
     for (int i = 0; i <= num_clusters; ++i) {
          for (int j = 0; j <= num_clusters; ++j) {               
               e[i][j] = 0.0;
          }
     }
     // Running through all clusters
     hmap_uint_suint::iterator mapIt;
     uint i,j;
     std::set<Edge> *adj;
     std::set<Edge>::iterator node;
     std::set<uint>::iterator setIt;
     for (mapIt = clusters->begin(); mapIt != clusters->end(); ++mapIt) {
          i = mapIt->first;
          // This will iterate the cluster i. We must get every one
          // of those nodes neighbors to know to which cluster they belong
          for (setIt = mapIt->second->begin();
               setIt != mapIt->second->end();
               ++setIt) {
               adj = graph->getAdjacency(*setIt);
               // Iterating the adjacency list of each cluster member
               for (node = adj->begin(); node != adj->end(); ++node) {
                    // No self loops, buddy!
                    if (*setIt != node->getNode()) {
                         j = (node_cluster->find(node->getNode()))->second;
                         // FIXME Na verdade é o "label" do cluster...
                         //e[i][j] += 1/(double)g.getNumEdges();
                         if (i == j) {
                              // Removing duplicated edges TODO What??
                              e[i][j] += 1/(double)(graph->getNumEdges()*2.0);
                         } else {
                              // No duplicates here
                              e[i][j] += 1/(double)graph->getNumEdges();
                         }
                    }
               }
          }
     }
     // Matrix e fully calculated
}

/********************************************************************
* Calculates the modularity for the clustering obtained. Obviously,
* The clustering must have already been made.
* IMPORTANT! We have a "virtual" cluster 0, that represents all hubs
* and outliers. It will exist here only so that we don't lose coherence
* of proportions, but e_{0,0} SHOULD NOT be considered for the trace Tr e.
* ... (I think)
********************************************************************/
// FIXME num_clusters ainda vai ter que ser +1?
float ClusterEvaluator::getModularity() {
     // 1 - Generate Matrix e, where:
     // e_{i,j} = (edges linking ci to cj)/(all edges in G)
     float** e;
     //std::cout << "num_clusters = " << num_clusters << std::endl;
     SquareMatrix<float> matrix(num_clusters+1);
     e = matrix;
     buildAssortativityMatrix(e);
     std::cout << "Matrix e:" << std::endl;
     printSquareMatrix(e, num_clusters+1);
     std::cout << "---------------------------------------" << std::endl;
     // 2 -  Calculate Trace(e) = SUM_{i} e_{ii}      
     float tr = 0.0;
     for (int i = 0; i <= num_clusters; ++i){
          tr += e[i][i];
     }
     std::cout << "Trace is: " << tr << std::endl;
     //std::cout << "Calculate trace (e): DONE!" << std::endl;
     // 3 - Calculate e^2
     SquareMatrix<float> matrix2(num_clusters+1);
     float** e_squared;
     e_squared = matrix2;
     squareMatrixMultiplication(e, e, e_squared, num_clusters+1);
     std::cout << "Matrix e squared:" << std::endl;
     printSquareMatrix(e_squared, num_clusters+1);
     std::cout << "---------------------------------------" << std::endl;
     //std::cout << "Calculate e^2: DONE!" << std::endl;
     // 4 - Sum all elements of e^2 (||e^2||)
     float sum_e = sumElementsSquareMatrix(e_squared, num_clusters+1);
     std::cout << "Sum is: " << sum_e << std::endl;
     //std::cout << "Sum all elements e^2 (||e^2||): DONE!" << std::endl;
     // 5 - Q = tr - ||e^2||
     std::cout << "Modularity is: " << tr - sum_e << std::endl;
     return tr - sum_e;
}


/*
 * TODO Ignore cluster 0?
 */
std::vector<double> ClusterEvaluator::getSilhouetteIndex() {
     hmap_i_i::iterator it;
     hmap_i_i spaths;
     hmap::iterator node;
     int ccount;
     double an = 0.0;
     int bn = MAXIMUM;
     std::vector<double> silhouette(clusters->size()+1);
     std::vector<int> node_count(clusters->size()+1);
     // Initializing average counters
     for (int i = 0; i < clusters->size()+1; ++i) {
          silhouette[i] = 0.0;
          node_count[i] = 0;
     }
     // For each node n in the graph
     unsigned int mycluster;
     for (node = graph->graph_map.begin(); node != graph->graph_map.end();
               ++node){
          // We will not calculate the Silhouette index for the
          // cluter 0, as it is not really a cluster
          mycluster = (node_cluster->find(node->first)->second);
          if (mycluster != 0) {
               // Calculate the shortest paths for all nodes
               spaths = graph->dijkstra(node->first);
               an = 0.0;
               ccount = 0;
               bn = MAXIMUM;
               for (it = spaths.begin(); it != spaths.end(); ++it) {
                    if (it->first != node->first){
                         if ((node_cluster->find(it->first))->second 
                                   == mycluster) {
                              // Calculate it's the average distance to the 
                              // other members of his cluster (an)
                              an += it->second;
                              ++ccount;
                         } else {
                              // Calculate smallest distance from him to any 
                              // other cluster (bn)
                              if (it->second < bn) bn = it->second;
                         }
                    }
               }
               an = an/(double)ccount;
               // His Silhouette index is Sn = (bn - an)/max(an, bn)
               silhouette[mycluster] += (bn - an)/
                    ((an > (double) bn)? an:(double)bn);
               node_count[mycluster] += 1;
               // TODO Final value will be the average?
               // Return the average value for each cluster. 
               //std::cout << "Silhouette index for node " << node->first <<
               //     " = " << to_string(silhouette) << std::endl; 
          }
     }
     for (int i = 1; i < silhouette.size(); ++i) {
          // Average Silhouette index for cluster i
          silhouette[i] = silhouette[i]/(double)node_count[i];
          // Average silhouete index for the whole graph
          silhouette[0] += silhouette[i]/(double)(silhouette.size()-1);
     }
     return silhouette;
}
