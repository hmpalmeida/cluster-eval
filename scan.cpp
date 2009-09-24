#include "scan.hpp"
#include <iostream>
#include <algorithm>
#include <iterator>
#include <math.h>
#include <queue>
#include <fstream>
#include <utility>


/********************************************************************
* Constructor
********************************************************************/
Scan::Scan(){
     num_clusters = 0;
     type = 1;
}

/********************************************************************
* Constructor
********************************************************************/
Scan::Scan(uint t){
     num_clusters = 0;
     type = t;
}

/********************************************************************
* Other constructor
********************************************************************/
Scan::Scan(uint t, std::string filename){
     num_clusters = 0;
     type = t;
     loadGraph(filename);
}

/********************************************************************
* Sets the similarity function to be used
********************************************************************/
void Scan::setSimFunction(uint t) {
     type = t;
}

/********************************************************************
* Loads a graph. Will decide which method should be used
********************************************************************/
void Scan::loadGraph(std::string filename){
     bool sloops = true;
     // This must change if we decide to remove self loops in other 
     // similarity functions
     if (type == 2) {
          sloops = false;
     }
     if (filename.compare(filename.size()-4,4,".gml") == 0) {
          try {
               g.readGmlFile(filename, sloops);
          } catch (std::string err) {
               std::cout << err << std::endl;
          }
     } else {
          try {
               g.readFile(filename, sloops);
          } catch (std::string err) {
               std::cout << err << std::endl;
          }
     }
}

/********************************************************************
* Main SCAN algorithm
********************************************************************/
void Scan::run(const double epsilon, const int mi){
     // All vertices begin unclassified
     // So let's begin. We will iterate the labels list. In no moment
     // can a label became "unclassified" again, so this loop will 
     // do the trick.
     hmap::iterator node;
     uint cnt = 0;
     for (node = g.graph_map.begin(); node != g.graph_map.end(); ++node) {
          // Making sure this node was not yet evaluated
          if (g.getLabel(node->first) == -1) {
               // Is it a core?
               if (isCore(node->first, epsilon, mi)) {
                    // Will begin a new cluster
                    int cid = getNewClusterID();
                    // Put all the e-neighborhood of the node in a queue
                    std::set<Edge> x;
                    x = neighborhood(node->first, epsilon);
                    std::queue<uint> q;
                    std::set<Edge>::iterator it;
                    for (it = x.begin(); it != x.end(); ++it) {
                         q.push(it->getNode());
                    }
                    while (!q.empty()) {
                         uint y = q.front();
                         q.pop();
                         std::set<Edge> r;
                         r = dirReach(y, epsilon, mi);
                         std::set<Edge>::iterator setIt;
                         for (setIt = r.begin(); setIt != r.end(); ++setIt) {
                              //std::cout << "Node " << *setIt << " | Label = " 
                              //     << g.getLabel(*setIt) << std::endl;
                              if (g.getLabel(setIt->getNode()) == -1) {
                                   addToCluster(setIt->getNode(), cid);
                                   q.push(setIt->getNode());
                              }
                              if (g.getLabel(setIt->getNode()) == 0) {
                                   addToCluster(setIt->getNode(), cid);
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
               tmp = getNeighborClusters(nmnode->first);
               if (tmp.size() > 1) {
                    addHub(nmnode->first, tmp); 
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

/********************************************************************
* Destructor
********************************************************************/
Scan::~Scan() {
     hmap_uint_suint::const_iterator it;
     for (it = clusters.begin(); it != clusters.end(); ++it) {
          delete it->second;
     }
          for (it = hubs.begin(); it != hubs.end(); ++it) {
          delete it->second;
     }
}


/********************************************************************
* Gathers data to form a useful output filename
********************************************************************/
std::string Scan::generateFilename(const double epsilon, const int mi) {
     std::string name;
     name = "results/epsilon-" + to_string(epsilon) + "_mi-" + 
          to_string(mi) + "_sim-" + to_string(type) + "_";
     return name;
}

/********************************************************************
* Prints all clusters. For testing
********************************************************************/
void Scan::writeClusters(std::string name){
     std::ofstream outfile(name.c_str());
     hmap_uint_suint::iterator mapIt;
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

/********************************************************************
* Prints all outliers
********************************************************************/
void Scan::writeOutliers(std::string name){
     //std::cout << "Outliers: " << std::endl;
     std::ofstream outfile(name.c_str());
     std::vector<std::pair<uint, uint> >::iterator it;
     for (it = outliers.begin(); it != outliers.end();++it){
          //std::cout << *it << "\t";
          outfile << it->first << " -> c" << it->second << std::endl;
     }
     outfile.close();
}

/********************************************************************
* Prints all hubs
********************************************************************/
void Scan::writeHubs(std::string name){
     //std::cout << "Hubs: " << std::endl;
     std::ofstream outfile(name.c_str());
     hmap_uint_suint::iterator mapIt;
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

/********************************************************************
* Prints everything
********************************************************************/
void Scan::writeAll(const double epsilon, const int mi){
     std::string name;
     name = generateFilename(epsilon, mi);
     writeClusters(name + "clusters.txt");
     writeHubs(name + "hubs.txt");
     writeOutliers(name + "outliers.txt");
}

/********************************************************************
* Prints the graph
********************************************************************/
void Scan::printGraph(){
     g.print();
}

/********************************************************************
* Returns the e-neighborhood of the given node
********************************************************************/
std::set<Edge> Scan::neighborhood(uint node, 
          const double epsilon){
     // Sets up the e-neighborhood counter
     uint count = 0;
     // Get the nodes adjacency list
     std::set<Edge> *adj;
     std::set<Edge> nhood;
     adj = g.getAdjacency(node);
     // Verify the node' similarity with it's neighbors.
     // And yes, he will check similarity with itself...
     std::set<Edge>::iterator setIt;
     for (setIt = adj->begin(); setIt != adj->end(); ++setIt) {
          if (similarity(node,setIt->getNode()) >= epsilon) {
               //std:: cout << "Sim " << node << " " << *setIt
               //     << " = " << similarity(node,*setIt) << std::endl;
               nhood.insert(*setIt);
          }
     }
     return nhood;

}

/********************************************************************
* Verifies if a node is a core
********************************************************************/
bool Scan::isCore(uint node, const double epsilon, const int mi){
     std::set<Edge> blah;
     blah = neighborhood(node, epsilon);
     if (blah.size() >= mi){
          return true;
     } else {
          return false;
     }
}

/********************************************************************
* Gets a new... aw, why bother?
********************************************************************/
int Scan::getNewClusterID() {
     ++num_clusters;
     return num_clusters;
}

/********************************************************************
* Calculates the Direct Structure Reachability [DirREACH(v,w)] 
* of a given node. Will return all w E N.
********************************************************************/
std::set<Edge> Scan::dirReach(uint v, const double epsilon, 
          const int mi) {
     std::set<Edge> s;
     if (isCore(v, epsilon, mi)) {
          s = neighborhood(v, epsilon);
     }
     return s;
}

/********************************************************************
* Creates a new hub
********************************************************************/
void Scan::addHub(uint node, std::set<uint> clusts) {
     //Creates the new hub
     hubs[node] = new std::set<uint>;
     hubs[node]->insert(clusts.begin(), clusts.end());
}

/********************************************************************
* Add a node to a cluster
********************************************************************/
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

/********************************************************************
* Used to verify if a node is a hub
********************************************************************/
std::set<uint> Scan::getNeighborClusters(uint node) {
     // This set will be used to verify how many clusters
     // this node has edges with
     std::set<uint> numcl;
     // Get the nodes adjacency list
     std::set<Edge> *adj;
     adj = g.getAdjacency(node);
     std::set<Edge>::iterator setIt;
     uint clust;
     // Will store all clusters adjacent to node. the set will deal
     // with any repetition
     for (setIt = adj->begin(); setIt != adj->end(); ++setIt) {
          if (node != setIt->getNode()) {
               clust = g.getLabel(setIt->getNode());
               if (clust > 0) numcl.insert(clust);
          }
     }
     return numcl;
}

/********************************************************************
* Returns the weight of the edge between two nodes
********************************************************************/
double Scan::getEdgeWeight(uint node1, uint node2) {
     Edge e(node2,0.0);
     std::set<Edge>::iterator i;
     return g.graph_map[node1]->find(e)->getWeight();
}

/********************************************************************
* Similarity functions
********************************************************************/

// TODO Vamos fazer diferente. Deixa a chamada de similarity para não 
// ter que mudar tudo e dentro dela chamar a função desejada.

/********************************************************************
* Original SCAN similarity function
********************************************************************/
double Scan::similarity(uint node1, uint node2){
     double sim = 0.0;
     switch (type) {
          case 1:
               // Traditional SCAN
               sim = scanSim(node1, node2);
               break;
          case 2:
               // SCAN without self loops
               sim = noSelfLoopSim(node1, node2);
               break;
          case 3:
               // Weighted SCAN
               sim = weightedSim(node1, node2);
               break;
          default:
               throw "Unknown similarity function.\n";
     }
     return sim;
}

/********************************************************************
* Original SCAN similarity function
********************************************************************/
double Scan::scanSim(uint node1, uint node2){
     // Variables
     std::set<Edge> *n1, *n2;
     std::set<Edge> inter;
     double divisor;
     n1 = g.getAdjacency(node1);
     n2 = g.getAdjacency(node2);
     // Calculate the intersection between  the edges' neighbors
     set_intersection(n1->begin(),n1->end(),
               n2->begin(), n2->end(),
               std::insert_iterator< std::set<Edge> >
               (inter, inter.begin()));
     divisor = n1->size() * n2->size();
     divisor = sqrt(divisor);
     return (inter.size()/(double)divisor);
}

/********************************************************************
* Adapted SCAN similarity function. No self links required
********************************************************************/
double Scan::noSelfLoopSim(uint node1, uint node2){
     // Variables
     std::set<Edge> *n1, *n2;
     std::set<Edge> inter;
     double divisor;
     n1 = g.getAdjacency(node1);
     n2 = g.getAdjacency(node2);
     // Calculate the intersection between  the edges' neighbors
     set_intersection(n1->begin(),n1->end(),
               n2->begin(), n2->end(),
               std::insert_iterator< std::set<Edge> >
               (inter, inter.begin()));
     divisor = n1->size() * n2->size();
     divisor = sqrt(divisor);
     return (inter.size()+1/(double)divisor);
}

/********************************************************************
* Simple weighted similarity function
********************************************************************/
double Scan::weightedSim(uint node1, uint node2){
     // Não faço a menor idéia de como fazer isso!
     // Variables
     std::set<Edge> *n1, *n2;
     std::set<Edge> inter;
     double divisor;
     n1 = g.getAdjacency(node1);
     n2 = g.getAdjacency(node2);
     // Calculate the intersection between  the edges' neighbors
     set_intersection(n1->begin(),n1->end(),
               n2->begin(), n2->end(),
               std::insert_iterator< std::set<Edge> >
               (inter, inter.begin()));
     divisor = n1->size() * n2->size();
     divisor = sqrt(divisor);
     divisor = (inter.size()/(double)divisor);
     // Ou seja, média simples entre a similaridade tradicional e o peso
     // da aresta entre os vértices
     return (divisor + getEdgeWeight(node1, node2))/2.0;
}




/********************************************************************
* Modularity Stuff
********************************************************************/

/********************************************************************
* Builds the assortativity marix used in modularity calculations
********************************************************************/
void Scan::buildAssortativityMatrix(float** e) {
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
                    if (*setIt != node->getNode()) {
                         j = g.getLabel(node->getNode());
                         //e[i][j] += 1/(double)g.getNumEdges();
                         if (i == j) {
                              // Removing duplicated edges
                              e[i][j] += 1/(double)(g.getNumEdges()*2.0);
                         } else {
                              // No duplicates here
                              e[i][j] += 1/(double)g.getNumEdges();
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
     //std::cout << "Calculate trace (e): DONE!" << std::endl;
     // 3 - Calculate e^2
     SquareMatrix<float> matrix2(num_clusters+1);
     float** e_squared;
     e_squared = matrix2;
     squareMatrixMultiplication(e, e_squared, num_clusters+1);
     //std::cout << "Calculate e^2: DONE!" << std::endl;
     // 4 - Sum all elements of e^2 (||e^2||)
     float sum_e = sumElementsSquareMatrix(e_squared, num_clusters+1);
     //std::cout << "Sum all elements e^2 (||e^2||): DONE!" << std::endl;
     // 5 - Q = tr - ||e^2||
     return tr - sum_e;
}
