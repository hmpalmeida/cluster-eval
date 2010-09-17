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
     // TODO Initialize all <cluster_label> with -1
     hmap::iterator it;
     for (it = g.graph_map.begin(); it != g.graph_map.end(); ++it) {
          cluster_label[it->first] = -1;
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
          if (getClusterLabel(node->first) == -1) {
               // Is it a core?
               if (isCore(node->first, epsilon, mi)) {
                    //std::cout << node->first << " is a core!\n";
                    // Will begin a new cluster
                    int cid = getNewClusterID();
                    // Put all the e-neighborhood of the node in a queue
                    std::set<Edge> x;
                    x = neighborhood(node->first, epsilon);
                    std::queue<uint> q;
                    std::set<Edge>::iterator it;
                    //std::cout << node->first << " nhood is:";
                    for (it = x.begin(); it != x.end(); ++it) {
                         //std::cout << "   " << it->getNode() << std::endl;
                         q.push(it->getNode());
                    }
                    while (!q.empty()) {
                         uint y = q.front();
                         q.pop();
                         std::set<Edge> r;
                         r = dirReach(y, epsilon, mi);
                         std::set<Edge>::iterator setIt;
                         for (setIt = r.begin(); setIt != r.end(); ++setIt) {
                              // If the node is unclassified
                              if (getClusterLabel(setIt->getNode()) == -1) {
                                   addToCluster(setIt->getNode(), cid);
                                   q.push(setIt->getNode());
                              }
                              // If the node is a non-member of the cluster
                              if (getClusterLabel(setIt->getNode()) == 0) {
                                   addToCluster(setIt->getNode(), cid);
                              }
                         }

                    }
               } else {
                    // Not a Core, so it will be labeled as non-member (0)
                    setClusterLabel(node->first, 0);
               }
          } else {
               // Node already evaluated. Move along.
               continue;
          }
     }
     // Further classifies non-members
     
     hmap_ii::iterator nmnode;
     for (nmnode = cluster_label.begin(); 
               nmnode != cluster_label.end(); ++nmnode) {
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
          outfile << " , " << g.getNodeLabel(*setIt);
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
          outfile << g.getNodeLabel(it->first) << 
               " -> c" << it->second << std::endl;
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
          outfile << g.getNodeLabel(mapIt->first) << " ->";
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
          if (similar(node,setIt->getNode(),epsilon)) {
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
     setClusterLabel(node, clust);
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
               clust = getClusterLabel(setIt->getNode());
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
// TODO Alterar a similaridade para retornar logo só o resultado.
// Isso vai permitir que eu use políticas hibridas, como um
// simScan >= e || weight >= e, ou algo do gênero.
bool Scan::similar(uint node1, uint node2, double epsilon){
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
               // Weighted mean SCAN
               sim = weightedMeanSim(node1, node2);
               break;
          case 4:
               // Weighted mean SCAN
               sim = weightedOnlySim(node1, node2);
               break;
          default:
               throw "Unknown similarity function.\n";
     }
     return (sim >= epsilon);
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
     //std::cout << "Sim(" << node1 << ", " << node2 << ") = " 
     //     << (inter.size()/(double)divisor) << std::endl;
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
double Scan::weightedMeanSim(uint node1, uint node2){
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
* Similarity function using only edge weight
********************************************************************/
double Scan::weightedOnlySim(uint node1, uint node2){
     return getEdgeWeight(node1, node2);
}

/********************************************************************
* Returns the value of a node's label (it's cluster)
********************************************************************/
long Scan::getClusterLabel(uint node){
     return cluster_label[node];
}

/********************************************************************
* Sets the value of a node's label
********************************************************************/
void Scan::setClusterLabel(uint node, long l){
     cluster_label[node] = l;
}

