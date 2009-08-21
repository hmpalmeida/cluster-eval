#include "scan.hpp"
#include <iostream>
#include <algorithm>
#include <iterator>
#include <math.h>
#include <queue>
#include <fstream>
#include <utility>

Scan::Scan(){
     num_clusters = 0;
}

Scan::Scan(std::string filename){
     num_clusters = 0;
     loadGraph(filename);
}

void Scan::loadGraph(std::string filename){
     try {
          g.readFile(filename);
     } catch (std::string err) {
          std::cout << err << std::endl;
     }
}

// Main SCAN algorithm
void Scan::run(const float epsilon, const int mi){
     // All vertices begin unclassified
     // So let's begin. We will iterate the labels list. In no moment
     // can a label became "unclassified" again, so this loop will 
     // do the trick.
     hmap::iterator node;
     uint cnt = 0;
     for (node = g.graph_map.begin(); node != g.graph_map.end(); ++node) {
          // Just so we don't get lost
          ++cnt;
          /*
          if (cnt % 10000 == 0){
               std::cout << cnt << " nodes evaluated." << std::endl;
          }
          */
          
          // Making sure this node was not yet evaluated
          if (g.getLabel(node->first) == -1) {
               // Is it a core?
               if (isCore(node->first, epsilon, mi)) {
                    // Will begin a new cluster
                    int cid = getNewClusterID();
                    // Put all the e-neighborhood of the node in a queue
                    std::set<uint> x;
                    x = neighborhood(node->first, epsilon);
                    std::queue<uint> q;
                    std::set<uint>::iterator it;
                    for (it = x.begin(); it != x.end(); ++it) {
                         q.push(*it);
                    }
                    while (!q.empty()) {
                         uint y = q.front();
                         q.pop();
                         std::set<uint> r;
                         r = dirReach(y, epsilon, mi);
                         std::set<uint>::iterator setIt;
                         for (setIt = r.begin(); setIt != r.end(); ++setIt) {
                              //std::cout << "Node " << *setIt << " | Label = " 
                              //     << g.getLabel(*setIt) << std::endl;
                              if (g.getLabel(*setIt) == -1) {
                                   addToCluster(*setIt, cid);
                                   q.push(*setIt);
                              }
                              if (g.getLabel(*setIt) == 0) {
                                   addToCluster(*setIt, cid);
                              }
                         }

                    }
               } else {
                    // Not a Core, so it will be labeled as non-member (0)
                    g.setLabel(node->first, 0);
               }
          } else {
               // Node already evaluated. Move along.
               continue;
          }
     }
     // Further classifies non-members
     
     hmap_ii::iterator nmnode;
     for (nmnode = g.label.begin(); nmnode != g.label.end(); ++nmnode) {
          if (nmnode->second == 0) {
               std::set<uint> tmp;
               tmp = isHub(nmnode->first);
               //if (isHub(nmnode->first)) {
               if (tmp.size() > 1) {
                    addHub(nmnode->first, tmp); 
                    //hubs.push_back(nmnode->first);
               } else {
                    std::pair<uint, uint> 
                         o(nmnode->first,*tmp.begin());
                    outliers.push_back(o);
               }
          }
     }
     std::cout << "SCAN finished.";
     std::cout << std::endl;
}

// Destructor
Scan::~Scan() {
   hmap::const_iterator it;
   for (it = clusters.begin(); it != clusters.end(); ++it) {
      delete it->second;
   }
   for (it = hubs.begin(); it != hubs.end(); ++it) {
      delete it->second;
   }
}

// Prints all clusters. For testing
void Scan::printClusters(){
     std::ofstream outfile("clusters.txt");
     hmap::iterator mapIt;
     std::set<uint>::iterator setIt;
     for (mapIt = clusters.begin(); mapIt != clusters.end(); ++mapIt) {
          outfile << "c" << mapIt->first << " ->";
          for (setIt = mapIt->second->begin();
               setIt != mapIt->second->end();
               ++setIt) {
          outfile << " " << *setIt;
          }
          outfile << std::endl;
     }
     outfile.close();
}

// Prints all outliers
void Scan::printOutliers(){
     //std::cout << "Outliers: " << std::endl;
     std::ofstream outfile("outliers.txt");
     std::vector<std::pair<uint, uint> >::iterator it;
     for (it = outliers.begin(); it != outliers.end();++it){
          //std::cout << *it << "\t";
          outfile << it->first << " -> c" << it->second << std::endl;
     }
     outfile.close();
}

// Prints all hubs
void Scan::printHubs(){
     //std::cout << "Hubs: " << std::endl;
     std::ofstream outfile("hubs.txt");
     hmap::iterator mapIt;
     std::set<uint>::iterator setIt;
     for (mapIt = hubs.begin(); mapIt != hubs.end(); ++mapIt) {
          //std::cout << mapIt->first << " ->";
          outfile << mapIt->first << " ->";
          for (setIt = mapIt->second->begin();
               setIt != mapIt->second->end();
               ++setIt) {
          //std::cout << " c" << *setIt;
          outfile << " c" << *setIt;
          }
          outfile << std::endl;
     }
     outfile.close();
}

// Prints everything
void Scan::printAll(){
     printClusters();
     printHubs();
     printOutliers();
}

// Prints the graph
void Scan::printGraph(){
     g.print();
}


// Calculates the similarity between two nodes
float Scan::similarity(uint node1, uint node2){
     // Variables
     std::set<uint> *n1, *n2;
     std::set<uint> inter;
     float divisor;
     n1 = g.getAdjacency(node1);
     n2 = g.getAdjacency(node2);
     // Calculate the intersection between  the edges' neighbors
     set_intersection(n1->begin(),n1->end(),
               n2->begin(), n2->end(),
               std::insert_iterator< std::set<uint> >
               (inter, inter.begin()));
     divisor = n1->size() * n2->size();
     divisor = sqrt(divisor);
     /*
     std::cout << "SIM(" << node1 << "," << node2 << ") = " << 
          (inter.size()/(float)divisor) << std::endl;
     */
     return (inter.size()/(float)divisor);
}

// Returns the e-neighborhood of the given node
std::set<uint> Scan::neighborhood(uint node, 
          const float epsilon){
     // Sets up the e-neighborhood counter
     uint count = 0;
     // Get the nodes adjacency list
     std::set<uint> *adj;
     std::set<uint> nhood;
     adj = g.getAdjacency(node);
     // Verify the node' similarity with it's neighbors.
     // And yes, he will check similarity with itself...
     std::set<uint>::iterator setIt;
     for (setIt = adj->begin(); setIt != adj->end(); ++setIt) {
          if (similarity(node,*setIt) >= epsilon) {
               //std:: cout << "Sim " << node << " " << *setIt
               //     << " = " << similarity(node,*setIt) << std::endl;
               nhood.insert(*setIt);
          }
     }
     return nhood;

}

// Verifies if a node is a core
bool Scan::isCore(uint node, const float epsilon, const int mi){
     std::set<uint> blah;
     blah = neighborhood(node, epsilon);
     if (blah.size() >= mi){
          return true;
     } else {
          return false;
     }
}

// Gets a new... aw, why bother?
int Scan::getNewClusterID() {
     ++num_clusters;
     return num_clusters;
}

// Calculates the Direct Structure Reachability [DirREACH(v,w)] 
// of a given node. Will return all w E N.
std::set<uint> Scan::dirReach(uint v, const float epsilon, 
          const int mi) {
     std::set<uint> s;
     if (isCore(v, epsilon, mi)) {
          s = neighborhood(v, epsilon);
     }
     return s;
}

void Scan::addHub(uint node, std::set<uint> clusts) {
     //Creates the new hub
     hubs[node] = new std::set<uint>;
     hubs[node]->insert(clusts.begin(), clusts.end());
}

void Scan::addToCluster(uint node, uint clust) {
     // Verify if it is a new cluster
     if (clusters[clust] == NULL) {
          // Set yet not created
          clusters[clust] = new std::set<uint>;
     }
     // Insert it. The set will handle possible duplicates
     clusters[clust]->insert(node);
     // Update the label
     g.setLabel(node, clust);
}

// Verifies if the node is a hub
std::set<uint> Scan::isHub(uint node) {
     // This set will be used to verify how many clusters
     // this node has edges with
     std::set<uint> numcl;
     // Get the nodes adjacency list
     std::set<uint> *adj;
     adj = g.getAdjacency(node);
     std::set<uint>::iterator setIt;
     uint clust;
     // Will store all clusters adjacent to node. the set will deal
     // with any repetition
     for (setIt = adj->begin(); setIt != adj->end(); ++setIt) {
          if (node != *setIt) {
               clust = g.getLabel(*setIt);
               if (clust > 0) numcl.insert(clust);
          }
     }
     return numcl;
}

void Scan::buildAssortativityMatrix(float** e) {
     // First, let's "zero" it
     for (int i = 0; i <= num_clusters; ++i) {
          for (int j = 0; j <= num_clusters; ++j) {               
               e[i][j] = 0.0;
          }
     }
     // Running through all clusters
     hmap::iterator mapIt;
     uint i,j;
     std::set<uint> *adj;
     std::set<uint>::iterator setIt, node;
     for (mapIt = clusters.begin(); mapIt != clusters.end(); ++mapIt) {
          i = mapIt->first;
          // This will iterate the cluster i. We must get every one
          // of this nodes neighbors to know to which cluster they belong
          for (setIt = mapIt->second->begin();
               setIt != mapIt->second->end();
               ++setIt) {
               adj = g.getAdjacency(*setIt);
               // Iterating the adjacency list of each cluster member
               for (node = adj->begin(); node != adj->end(); ++node) {
                    // No self loops, buddy!
                    if (*setIt != *node) {
                         j = g.getLabel(*node);
                         //e[i][j] += 1/(float)g.getNumEdges();
                         if (i == j) {
                              // Removing duplicated edges
                              e[i][j] += 1/(float)(g.getNumEdges()*2.0);
                         } else {
                              // No duplicates here
                              e[i][j] += 1/(float)g.getNumEdges();
                         }
                    }
               }
          }
     }
     // Matrix e fully calculated
}

// Calculates the modularity for the clustering obtained. Obviously,
// The clustering must have already been made.
// IMPORTANT! We have a "virtual" cluster 0, that represents all hubs
// and outliers. It will exist here only so that we don't lose coherence
// of proportions, but e_{0,0} SHOULD NOT be considered for the trace Tr e.
// ... (I think)
float Scan::getModularity() {
     // 1 - Generate Matrix e, where:
     // e_{i,j} = (edges linking ci to cj)/(all edges in G)
     float** e;
     std::cout << "num_clusters = " << num_clusters << std::endl;
     SquareMatrix<float> matrix(num_clusters+1);
     e = matrix;
     buildAssortativityMatrix(e);
     // 2 -  Calculate Trace(e) = SUM_{i} e_{ii}      
     float tr = 0.0;
     for (int i = 0; i <= num_clusters; ++i){
          tr += e[i][i];
     }
     std::cout << "Calculate trace (e): DONE!" << std::endl;
     // 3 - Calculate e^2
     SquareMatrix<float> matrix2(num_clusters+1);
     float** e_squared;
     e_squared = matrix2;
     squareMatrixMultiplication(e, e_squared, num_clusters+1);
     std::cout << "Calculate e^2: DONE!" << std::endl;
     // 4 - Sum all elements of e^2 (||e^2||)
     float sum_e = sumElementsSquareMatrix(e_squared, num_clusters+1);
     std::cout << "Sum all elements e^2 (||e^2||): DONE!" << std::endl;
     // 5 - Q = tr - ||e^2||
     return tr - sum_e;
}
