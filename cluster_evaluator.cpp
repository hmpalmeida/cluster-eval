#include "utils.hpp"
#include "cluster_evaluator.hpp"
#include <cmath>

/********************************************************************
* Modularity Stuff
********************************************************************/

ClusterEvaluator::ClusterEvaluator() {
}

ClusterEvaluator::ClusterEvaluator(Graph* g) {
     loadGraph(g);
}

ClusterEvaluator::ClusterEvaluator(Graph* g, hmap_uint_suint* cls, 
          hmap_uint_suint* nc, std::vector<std::string>* clbs, uint vnn) {
     loadGraph(g);
     loadClusters(cls, nc);
     cluster_labels = clbs;
     v_num_nodes = vnn;
}

ClusterEvaluator::~ClusterEvaluator() {
}

void ClusterEvaluator::loadGraph(Graph* g) {
     graph = g;
}

void ClusterEvaluator::loadClusters(hmap_uint_suint* cls, 
          hmap_uint_suint* nc) {
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
     uint edgecount = 0;
     for (mapIt = clusters->begin(); mapIt != clusters->end(); ++mapIt) {
          i = mapIt->first;
          // This will iterate the cluster i. We must get every one
          // of those nodes neighbors to know to which cluster they belong
          std::set<uint>::iterator itcl;
          std::set<uint>* clusts;
          for (setIt = mapIt->second->begin();
               setIt != mapIt->second->end();
               ++setIt) {
               adj = graph->getAdjacency(*setIt);
               // Iterating the adjacency list of each cluster member
               for (node = adj->begin(); node != adj->end(); ++node) {
                    // No self loops, buddy!
                    if (*setIt != node->getNode()) {
                         // All the clusters of node
                         clusts = node_cluster->find(node->getNode())->second;
                         for (itcl = clusts->begin(); itcl != clusts->end();
                                   ++itcl) { 
                              j = *itcl;
                              e[i][j] += 1;
                              ++edgecount;
                              /*
                              if (i == j) {
                                   // Removing duplicated edges
                                   e[i][j] +=1/(double)(graph->getNumEdges()*2);
                              } else {
                                   // No duplicates here
                                   e[i][j] += 1/(double)graph->getNumEdges();
                              }
                              */
                         }
                    }
               }
          }
     }
     for (int i = 0; i <= num_clusters; ++i) {
          for (int j = 0; j <= num_clusters; ++j) {               
               e[i][j] = e[i][j]/(double)edgecount;;
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
float ClusterEvaluator::getModularity() {
     // 1 - Generate Matrix e, where:
     // e_{i,j} = (edges linking ci to cj)/(all edges in G)
     float** e;
     //std::cout << "num_clusters = " << num_clusters << std::endl;
     SquareMatrix<float> matrix(num_clusters+1);
     e = matrix;
     buildAssortativityMatrix(e);
     //std::cout << "Matrix e:" << std::endl;
     //printSquareMatrix(e, num_clusters+1);
     //std::cout << "---------------------------------------" << std::endl;
     // 2 -  Calculate Trace(e) = SUM_{i} e_{ii}      
     float tr = 0.0;
     for (int i = 0; i <= num_clusters; ++i){
          tr += e[i][i];
     }
     //std::cout << "Trace is: " << tr << std::endl;
     //std::cout << "Calculate trace (e): DONE!" << std::endl;
     // 3 - Calculate e^2
     SquareMatrix<float> matrix2(num_clusters+1);
     float** e_squared;
     e_squared = matrix2;
     squareMatrixMultiplication(e, e, e_squared, num_clusters+1);
     //std::cout << "Matrix e squared:" << std::endl;
     //printSquareMatrix(e_squared, num_clusters+1);
     //std::cout << "---------------------------------------" << std::endl;
     //std::cout << "Calculate e^2: DONE!" << std::endl;
     // 4 - Sum all elements of e^2 (||e^2||)
     float sum_e = sumElementsSquareMatrix(e_squared, num_clusters+1);
     //std::cout << "Sum is: " << sum_e << std::endl;
     //std::cout << "Sum all elements e^2 (||e^2||): DONE!" << std::endl;
     // 5 - Q = tr - ||e^2||
     //std::cout << "Modularity is: " << tr - sum_e << std::endl;
     return tr - sum_e;
}


/*
 * Calculates the Silhouette index for a graph. Returns a vector
 * with the SI for the whole graph in position 0 and for each cluster
 * in the position of it's identifier.
 */
std::vector<double> ClusterEvaluator::getSilhouetteIndex() {
     hmap_i_i::iterator it;
     hmap_i_i spaths;
     hmap::iterator node;
     //int ccount;
     double an = 0.0;
     int bn = MAXIMUM;
     hmap_i_i ccount;
     hmap_i_i acc;
     std::vector<double> silhouette(clusters->size()+1);
     std::vector<int> node_count(clusters->size()+1);
     // Initializing average counters
     for (int i = 0; i < clusters->size()+1; ++i) {
          silhouette[i] = 0.0;
          node_count[i] = 0;
     }
     // For each node n in the graph
     unsigned int mycluster;
     std::set<uint>* ctmp;
     std::set<uint>::iterator itcl;
     for (node = graph->graph_map.begin(); node != graph->graph_map.end();
               ++node){
          // We will not calculate the Silhouette index for the
          // cluster 0, as it is not really a cluster
          std::set<uint>::iterator sit;
          for (sit = node_cluster->find(node->first)->second->begin();
                    sit != node_cluster->find(node->first)->second->end();
                    ++sit) {
               //mycluster = (node_cluster->find(node->first)->second);
               mycluster = *sit;
               if (mycluster != 0) {
                    // Calculate the shortest paths for all nodes
                    spaths = graph->dijkstra(node->first);
                    acc.clear();
                    ccount.clear();
                    for (it = spaths.begin(); it != spaths.end(); ++it) {
                         if (it->first != node->first){
                              ctmp = node_cluster->find(it->first)->second;
                              // Iterate through all of its clusters
                              for (itcl = ctmp->begin(); itcl != ctmp->end();
                                        ++itcl) { 
                                   if (acc.find(*itcl) == acc.end()) {
                                        // New cluster!
                                        acc[*itcl] = it->second;
                                        ccount[*itcl] = 1;
                                   } else {
                                        // Already exists. Accumulate
                                        acc[*itcl] = acc[*itcl] + it->second;
                                        ccount[*itcl] = ccount[*itcl] + 1;
                                   }
                              }
                         }
                    }
                    // Calculate average cluster distances
                    an = 0.0;
                    bn = MAXIMUM;
                    for (it = acc.begin(); it != acc.end(); ++it) {
                         if (it->first == mycluster) {
                              // Average distance to own cluster
                              an = it->second/(double)ccount[it->first];
                         } else {
                              // Minimum average distance to other clusters
                              double btmp = it->second/
                                   (double)ccount[it->first];
                              if (btmp < bn) bn = btmp;
                         }
                    }
                    //an = an/(double)ccount;
                    // His Silhouette index is Sn = (bn - an)/max(an, bn)
                    silhouette[mycluster] += (bn - an)/
                         ((an > (double) bn)? an:(double)bn);
                    node_count[mycluster] += 1;
               }
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

// The entropy calculus is basically the one used in Yang Zhou's
// "Graph Clustering Based on Structural/Attribute Similarities"
double ClusterEvaluator::getGraphEntropy() {
     double entropy = 0;
     // Get label weights. Those will be given by their replication
     hmap_s_i voc_weights; 
     std::set<std::string>* voc;
     std::set<std::string>::iterator vit;
     hmap::iterator it;
     uint num_entries = 0;
     for (it = graph->graph_map.begin(); 
               it != graph->graph_map.end(); ++it) {
          // Get node Vocabulary
          voc = graph->getVocabulary(it->first);
          // Insert elements in the global cluster vocabulary set
          for (vit = voc->begin(); vit != voc->end(); ++vit) {
               if (voc_weights.find(*vit) == voc_weights.end()) {
                    voc_weights[*vit] = 1;
               } else {
                    voc_weights[*vit] = voc_weights[*vit] + 1;
               }
               ++ num_entries;
          }
     }
     // Iterating through each cluster
     hmap_uint_suint::iterator it2;
     std::set<uint>::iterator sit;
     for (it2 = clusters->begin(); it2 != clusters->end(); ++it2) {
          uint cid = it2->first;
          //std::cout << "----------------------------------------------\n";
          double cent = getClusterEntropy(cid, &voc_weights, num_entries);
          std::cout << "Entropy for cluster " << cid << " (" << 
               cluster_labels->at(cid) << "): " << cent << std::endl;
          entropy += cent;
     }
     return entropy;
}

double ClusterEvaluator::getClusterEntropy(uint cid, hmap_s_i* vocw, 
          uint num_entries_g) {
     // Gather all terms of all elements in the cluster
     hmap_s_i gcvoc; 
     std::set<std::string>* voc;
     std::set<std::string>::iterator vit;
     std::set<uint>::iterator it;
     uint num_entries = 0;
     std::set<uint>* celements = clusters->find(cid)->second;
     for (it = celements->begin(); it != celements->end(); ++it) {
          // Get node Vocabulary
          voc = graph->getVocabulary(*it);
          // Insert elements in the global cluster vocabulary set
          for (vit = voc->begin(); vit != voc->end(); ++vit) {
               if (gcvoc.find(*vit) == gcvoc.end()) {
                    gcvoc[*vit] = 1;
               } else {
                    gcvoc[*vit] = gcvoc[*vit] + 1;
               }
               ++ num_entries;
          }
     }
     // For each of those terms, get its entropy in the cluster
     hmap_s_i::iterator gcvit;
     double c_entropy = 0;
     // Cluster <cid>'s relative size
     double c_rel_size = clusters->find(cid)->second->size()/
          (double)graph->graph_map.size();
     //std::cout << "Cluster " << cid << " relative size: "<< c_rel_size 
     //     << std::endl;
     //c_rel_size = c_rel_size/(double)graph->getNumNodes();
     c_rel_size = c_rel_size/(double)(v_num_nodes + graph->getNumNodes());
     for (gcvit = gcvoc.begin(); gcvit != gcvoc.end(); ++gcvit) {
          // Calculate term weight relative to the whole graph
          //double tw = vocw->find(gcvit->first)->second/(double)num_entries_g;
          double tw = 1;
          //std::cout << "Tw[" << gcvit->first << "] = " << 
          //     vocw->find(gcvit->first)->second << " / " << num_entries_g <<
          //     " = " << tw << std::endl;
          // Calculate term entropy in the cluster
          //double tmp = gcvit->second/(double)num_entries;
          double tmp = gcvit->second/
               (double)clusters->find(cid)->second->size();
          //std::cout << "Prob(" << gcvit->first << ") = " << tmp << std::endl;
          //tw = tw * (-1) * tmp * log2(tmp);// TODO entropy(ai, vj)
          tw = tw * (-1) * tmp * log10(tmp);// TODO entropy(ai, vj)
          //std::cout << "Entropy for term " << gcvit->first << " = " <<
          //     tw << std::endl;
          c_entropy += tw;
     }
     c_entropy = c_entropy * c_rel_size;
     return c_entropy;
}

/******************************************************
  * Conductance calculus functions
  ****************************************************/
// Also known as alpha(C), where C is a clustering C1, C2, ... Ck
double getIntraclusterConductance() {
     // It's the minimum of phi(G[Ci]), where G([Ci]) is the subgraph
     // induced by the vertices from cluster Ci (graph phi)
}

// Also known as sigma(C), with C being the same thing as in alpha
double getInterclusterConductance() {
     // It's 1 - the maximum value of phi(Ci) (cluster phi)
}
