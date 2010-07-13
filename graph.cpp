#include "graph.hpp"
#include <iostream>
#include <exception>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <math.h>
#include <algorithm>
#include <queue>
#include <utility>


/********************************************************************
* Just an empty constructor
********************************************************************/
Graph::Graph(){
     greatest_weight = -999999.0;
     smallest_weight = 999999.0;
     directed = false;
}

/********************************************************************
* Constructor that automatically loads the graph from a file
********************************************************************/
Graph::Graph(std::string filename){
     greatest_weight = -9999.0;
     smallest_weight = 9999.0;
     directed = false;
     if (filename.compare(filename.size()-4,4,".gml") == 0) {
          readGmlFile(filename);
     } else {
          readFile(filename);
     }
}

/********************************************************************
* Reads a graph from a gml file
*********************************************************************/
void Graph::readGmlFile(std::string filename) {
     // auxiliar vars
     std::string attr;
     std::string value;
     std::ifstream file;
     double weight = 1;
     int braces = 0, source = -1, target = -1;
     uint nid = 0;
     char tmp[256]; 
     
     num_edges = 0;
     
     // opening file
     file.open(filename.c_str());
     if (file.is_open()) {
          // Ignore everything untill we get the "graph" starting command
          do {
               file >> attr;
          } while (attr != "graph");
          // Graph definition has begun, now the next line must have a "["
          file >> attr;
          if (attr !=  "[") {
               throw ("Wrong format. Expecting \"[\" after \"graph\".\n");
          }
          ++braces;
          // Now begin reading the graph
          while ((braces > 0) || (file.eof())) {
               file >> attr;
               if (attr == "directed") {
                    // Control attribute. Register!
                    file >> value;
                    if (value == "1") {
                         directed = true;
                    }
               } else if (attr == "node") {
                    // Found a node definition. Everything must be 
                    // inside braces
                    file >> attr;
                    if (attr != "[") {
                         throw ("Wrong format. Expecting \"[\" after \"node\".\n");
                    }
                    ++braces;
                    // Get one more line
                    file >> attr;
                    //Node n = new Node();
                    while (attr != "]") {
                         // For now, the node id and attributes are the only keys 
                         // we accept. We will skip anything else
                         if (attr == "id") {
                              file >> value;
                              nid = atoi(value.c_str());
                              graph_map[nid] = NULL;
                              graph_attributes[nid] = NULL;
                              // Setting the id as the label, for the case
                              // that the nodehas no labels
                              graph_labels[nid] = value;
                              //label_id[value] = nid;
                         } else if (attr == "attributes") {
                              file >> attr;
                              if (attr != "[") {
                                   throw ("Wrong format. Expecting \"[\" after \"attributes\".\n");
                              }
                              if (graph_attributes[nid] == NULL) {
                                   graph_attributes[nid] = new std::set<std::string>;
                              }
                              file >> value;
                              while (value != "]") { 
                                   graph_attributes[nid]->insert(value);
                                   file >> value;
                              }
                         } else if (attr == "label") {
                              file.getline(tmp, 256);
                              value = tmp;
                              value.erase(0, 1);
                              graph_labels[nid] = value;
                              label_id[value] = nid;
                         }
                         // Rinse. Repeat
                         file >> attr;
                    }
                    --braces;
                    continue;
               } else if (attr == "edge") {
                    // Found an edge definition. Everything must be 
                    // inside braces
                    file >> attr;
                    if (attr != "[") {
                         throw ("Wrong format. Expecting \"[\" after \"edge\".\n");
                    }
                    ++braces;
                    // Get one more line
                    file >> attr;
                    while (attr != "]") {
                         if (attr == "source") {
                              file >> value;
                              source = atoi(value.c_str());
                         } else if (attr == "target") {
                              file >> value;
                              target = atoi(value.c_str());
                         } else if (attr == "value") {
                              file >> value;
                              weight = atof(value.c_str());
                         }
                         // Rinse. Repeat
                         file >> attr;
                    }
                    // Adding the edge read
                    addEdge(source, target, weight);
                    if (!directed) addEdge(target, source, weight);
                    ++num_edges;
                    --braces;
                    continue;
               } else if (attr == "]") {
                    --braces;
               } else {
                    throw ("Unknown term: ", attr);
               }
          }

     }
     if (braces != 0) {
          throw ("Error: Brace mismatch.");
     }
     file.close();
     // Normalize weights
     normalizeWeights();
     num_nodes = graph_map.size();
}


/********************************************************************
* Expects a file formatted like this::
* nodes (num_nodes) dir (1|0) weight (1|0)
* node_id
* ...
* edges
* node1 node2 (weight)
* ...
********************************************************************/
void Graph::readFile(std::string filename){
     // auxiliar vars
     std::string tmp, dir, weig;
     bool weighted;
     uint auxint, auxint2;
     
     std::ifstream file;
     double auxdouble;
     
     num_edges = 0;

     // opening file
     file.open(filename.c_str());
     if (file.is_open()) {
          // first line should start with "nodes" and the number of nodes
          // tests.google.com (parte do code.google.com, o nome pode 
          // não ser esse)
          //fscanf(file, "%s %d %s %c %s %c", &tmp, &auxint, &dir, &directed,
          //                                   &weig, &weighted);
          file >> tmp; file >> auxint;
          file >> dir; file >> directed;
          file >> weig; file >> weighted;
          // Checking the file format
          if ((tmp  != "nodes") ||
              (dir  != "dir")   ||
              (weig != "weight")) {
               throw ("Input in wrong format: expecting \"nodes (num_nodes) dir (1|0) weight (1|0) \".");
          }
          // Everything ok? Proceed reading the nodes
          num_nodes = auxint;
          for (int i = 0; i < num_nodes; ++i) {
               //fscanf(file, "%d", &auxint);
               file >> auxint;
               graph_map[auxint] = NULL;
          }
          // and now, the edges
          file >> tmp;
          if (tmp != "edges") {
               throw ("Input in wrong format: expecting \"edges\"");
          }
          // Fugly, I know, but gets the job done
          if ((!weighted) && (!directed)) {
               while (!file.eof()) {
                    file >> auxint; file >> auxint2;
                    addEdge(auxint, auxint2);
                    addEdge(auxint2, auxint);
                    ++num_edges;
               }
          } else if ((!weighted) && (directed)) {
               while (!file.eof()) {
                    file >> auxint; file >> auxint2;
                    addEdge(auxint, auxint2);
                    ++num_edges;
               }
          } else if ((weighted) && (!directed)) {
               while (!file.eof()) {
                    file >> auxint; file >> auxint2; file >> auxdouble;
                    addEdge(auxint, auxint2, auxdouble);
                    addEdge(auxint2, auxint, auxdouble);
                    ++num_edges;
               }
          } else {
               while (!file.eof()) {
                    file >> auxint; file >> auxint2; file >> auxdouble;
                    addEdge(auxint, auxint2, auxdouble);
                    ++num_edges;
               }
          }
          file.close();
          // Normalize weights
          normalizeWeights();
     } else {
          // File did not open... report
          throw ("Error opening file.");
     }
}

/********************************************************************
* is necessary, add it both ways. Also, needs to use the "Edge" class
* Add a DIRECTED edge to the graph. Must Add both ways if undirected
********************************************************************/
void Graph::addEdge(uint node1, uint node2, double weight){
     // adding reference to the first node
     if (graph_map[node1] == NULL) {
          // Set yet not created
          graph_map[node1] = new std::set<Edge>;
     }
     // insert it. The set will handle possible duplicates
     Edge e;
     e.setNode(node2);
     e.setWeight(weight);
     graph_map[node1]->insert(e);
     // Evaulate if it has the greatest or smallest weight
     if (weight > greatest_weight) {
          greatest_weight = weight;
     }
     if (weight < smallest_weight) {
          smallest_weight = weight;
     }
}

// Destructor
Graph::~Graph() {
     hmap::const_iterator it;
     for (it = graph_map.begin(); it != graph_map.end(); ++it) {
          // Now, Delete the set
          delete it->second;
     }
     hmap_i_ss::iterator it2;
     for (it2 = graph_attributes.begin(); it2 != graph_attributes.end(); 
               ++it2) {
          if (it2->second != NULL) {
               // Now, Delete the set
               delete it2->second;
          }
     }
}

/********************************************************************
* Print the vector. Mainly for testing purposes
********************************************************************/
void Graph::print() {
     hmap::iterator mapIt;
     std::set<Edge>::iterator setIt;
     for (mapIt = graph_map.begin(); mapIt != graph_map.end(); ++mapIt) {
          std::cout << mapIt->first << " ->";
          if (mapIt->second != NULL) {
               for (setIt = mapIt->second->begin();
                    setIt != mapIt->second->end();
                    ++setIt) {
                    std::cout << " " << setIt->toString();
               }
          }
          std::cout << std::endl;
     }
}

/********************************************************************
* Print all edge weights. Mainly for testing purposes
********************************************************************/
void Graph::printWeights() {
     hmap::iterator mapIt;
     std::set<Edge>::iterator setIt;
     for (mapIt = graph_map.begin(); mapIt != graph_map.end(); ++mapIt) {
          if (mapIt->second != NULL) {
               for (setIt = mapIt->second->begin();
                    setIt != mapIt->second->end();
                    ++setIt) {
                    std::cout << setIt->getWeight() << std::endl;
               }
          }
     }
}

/********************************************************************
* Returns the adjacency list
********************************************************************/
std::set<Edge>* Graph::getAdjacency(uint node){
     return graph_map[node];
}

/********************************************************************
* Returns the graph's number of edges
********************************************************************/
uint Graph::getNumEdges() {
     return num_edges;
}

/********************************************************************
* Returns the graph's number of nodes
********************************************************************/
uint Graph::getNumNodes() {
     return num_nodes;
}

/********************************************************************
* Normalizes the edge weights, if they exist
********************************************************************/
void Graph::normalizeWeights() {
     // If the graph is weighted
     if ((greatest_weight != 0) || (smallest_weight != 0)) {
          hmap::iterator mapIt;
          std::set<Edge>::iterator setIt;
          for (mapIt = graph_map.begin(); mapIt != graph_map.end(); ++mapIt) {
               if (mapIt->second != NULL) {
                    for (setIt = mapIt->second->begin();
                              setIt != mapIt->second->end();
                              ++setIt) {
                         // FIXME Isso tá feio pra caralho. É preciso descobrir
                         // o porque do iterador estar insistindo em dizer que
                         // é const quando ele claramente não o é!
                         Edge* e = (Edge*)&(*setIt);
                         double w;
                         w = e->getWeight();
                         // If there are negative weights, add the absolute
                         // value of the lowest weight to all edges
                         if (smallest_weight < 0) {
                              w += fabs(smallest_weight);
                         }
                         // Normalize
                         w = w/(double)greatest_weight;
                         e->setWeight(w);                      
                    }
               }
          }
     }
}

/********************************************************************
* Returns the <node>'s vocabulary
********************************************************************/
std::set<std::string>* Graph::getVocabulary(uint node){
     return graph_attributes[node];
}

/********************************************************************
* Verify if <node> knows <word>
********************************************************************/
bool Graph::hasWord(uint node, std::string word) {
     if (graph_attributes[node]->find(word) == graph_attributes[node]->end()) {
          // <node> does not know about the bird
          return false;
     } else {
          // <node> knows that the bird is the word
          return true;
     }
}


/********************************************************************
* Adds <word> to <node>'s vocabulary
********************************************************************/
void Graph::addWord(uint node, std::string word) {
     graph_attributes[node]->insert(word);
}


/********************************************************************
* Remove all words from <node>'s vocabulary
********************************************************************/
void Graph::dropVocabulary(uint node) {
     graph_attributes[node]->clear();
}


/********************************************************************
* Print all nodes and their vocabularies
********************************************************************/
void Graph::printVocabularies() {
     hmap_i_ss::iterator it;
     std::set<std::string>::iterator vocIt;
     for (it = graph_attributes.begin(); 
             it != graph_attributes.end(); ++it) {
          std::cout << graph_labels[it->first] << "  ->  ";
          for (vocIt = it->second->begin(); 
                    vocIt != it->second->end(); 
                    ++vocIt) {
               std::cout << *vocIt << "   ";
          }
          std::cout << std::endl;
     }
}

std::string Graph::getNodeLabel(uint node) {
     if (graph_labels.find(node) != graph_labels.end()) {
          return graph_labels[node];
     } else {
          return to_string<uint>(node);
     }
}

/********************************************************************
* Print the vector. Mainly for testing purposes
********************************************************************/
void Graph::printDotFile(std::string filename) {
     std::fstream file(filename.c_str(), std::ios::out);
     std::string edge(" -- ");
     (directed) ? edge = " -> " : edge = " -- ";
     hmap::iterator mapIt;
     std::set<Edge>::iterator setIt;
     filename.erase(filename.size() - 4);
     file << "graph " << filename << " { " << std::endl;
     // First the nodes and it's labels
     for (mapIt = graph_map.begin(); mapIt != graph_map.end(); ++mapIt) {
          file << "\t" << mapIt->first << " [label = \"" << 
               graph_labels[mapIt->first] << "\"];" << std::endl; 
     }
     // Now the edges
     if (directed) {
          for (mapIt = graph_map.begin(); mapIt != graph_map.end(); ++mapIt) {
               if (mapIt->second != NULL) {
                    for (setIt = mapIt->second->begin();
                              setIt != mapIt->second->end();
                              ++setIt) {
                         file << "\t" << mapIt->first << edge << 
                         setIt->getNode() << ";" << std::endl;
                    }
               }
          }
     } else {
          std::string tmp;
          std::set<std::string> tmp_set;
          for (mapIt = graph_map.begin(); mapIt != graph_map.end(); ++mapIt) {
               if (mapIt->second != NULL) {
                    for (setIt = mapIt->second->begin();
                         setIt != mapIt->second->end();
                         ++setIt) {
                         if (mapIt->first < setIt->getNode()) {
                         tmp = "\t" + to_string<uint>(mapIt->first) + edge + 
                              to_string<uint>(setIt->getNode()) + ";";
                         } else {
                         tmp = "\t" + to_string<uint>(setIt->getNode()) + edge
                              + to_string<uint>(mapIt->first) + ";";
                         }
                         tmp_set.insert(tmp);
                    }
               }
          }
          std::set<std::string>::iterator ssit;
          for (ssit = tmp_set.begin(); ssit != tmp_set.end(); ++ssit) {
               file << *ssit << std::endl;
          }
     }
     file << "}" << std::endl;
     file.close();
}

void Graph::printDegreeDistroData() {
     hmap::iterator it;
     std::vector<uint> degrees;
     std::fstream output("deg-distro.txt", std::ios::out);
     for (it = graph_map.begin(); it != graph_map.end(); ++it) {
          degrees.push_back(it->second->size());
     }
     std::sort(degrees.begin(), degrees.end());
     uint val = 0;
     for (int i = 0; i < degrees.size(); ++i) {
          if (degrees[i] > val) {
               output << degrees[i] << "\t" << 1-(i/(double)num_nodes) 
                    << std::endl;
               val = degrees[i];
          }
     }
     output.close();
}


void Graph::printOtterFile(std::string filename) {
     std::fstream file(filename.c_str(), std::ios::out);
     hmap::iterator mapIt;
     std::set<Edge>::iterator setIt;
     file << "t " << num_nodes << std::endl;
     if (directed) {
          file << "T " << num_edges << std::endl;
     } else {
          file << "T " << num_edges/2 << std::endl;
     }
     // First the nodes and it's labels
     for (mapIt = graph_map.begin(); mapIt != graph_map.end(); ++mapIt) {
          // Labels cannot have empty spaces..
          std::string tmp = graph_labels[mapIt->first];
          for (int i = 0; i < tmp.size(); ++i) {
               if (tmp[i] == ' ') tmp[i] = '_';
          }
          file << "? " << mapIt->first << " " << tmp << std::endl; 
     }
     // Now the edges
     if (directed) {
          for (mapIt = graph_map.begin(); mapIt != graph_map.end(); ++mapIt) {
               if (mapIt->second != NULL) {
                    for (setIt = mapIt->second->begin();
                              setIt != mapIt->second->end();
                              ++setIt) {
                         file << "l " << mapIt->first << " " <<
                              setIt->getNode() << std::endl;
                    }
               }
          }
     } else {
          std::string tmp;
          std::set<std::string> tmp_set;
          for (mapIt = graph_map.begin(); mapIt != graph_map.end(); ++mapIt) {
               if (mapIt->second != NULL) {
                    for (setIt = mapIt->second->begin();
                         setIt != mapIt->second->end();
                         ++setIt) {
                         if (mapIt->first < setIt->getNode()) {
                         tmp = to_string<uint>(mapIt->first) + " " + 
                              to_string<uint>(setIt->getNode());
                         } else {
                         tmp = to_string<uint>(setIt->getNode()) +
                              " " + to_string<uint>(mapIt->first);
                         }
                         tmp_set.insert(tmp);
                    }
               }
          }
          uint index = 0;
          std::set<std::string>::iterator ssit;
          for (ssit = tmp_set.begin(); ssit != tmp_set.end(); ++ssit) {
               file << "L " << index << " " << *ssit << std::endl;
               ++index;
          }
     }
     file.close();
}

bool Graph::isDirected() {
     return directed;
}

/*
 * Prints the graph in a format understandable by the cluto graph suite
 * from Karypis lab
 */
void Graph::printClutoFile(std::string filename) {
     std::fstream file(filename.c_str(), std::ios::out);
     hmap::iterator mapIt;
     std::set<Edge>::iterator setIt;
     file << num_nodes << " " << num_edges << std::endl;
     // First the nodes and it's labels
     for (mapIt = graph_map.begin(); mapIt != graph_map.end(); ++mapIt) {
          if (mapIt->second != NULL) {
               for (setIt = mapIt->second->begin();
                         setIt != mapIt->second->end();
                         ++setIt) {
                    Edge e = *setIt;
                    file << e.getNode() + 1 << " " << e.getWeight() << " "; 
               }
               file << std::endl;
          } else {
               std::cout << "Isolated node (" << mapIt->first << 
                    ") should not be here!" << std::endl;
               exit(1);
          }
     }
     file.close();
}

/*
 *
 */

hmap_i_i Graph::dijkstra(unsigned int source) {
     hmap_i_i distance;          
     std::set<unsigned int> visited;
     hmap::iterator it;
     std::pair<uint, uint> node;
     std::priority_queue< std::pair<uint, uint> > q;
     
     // Initializations
     for (it = graph_map.begin(); it != graph_map.end(); ++it) {
          // Unknown distance function from source to v
          distance[it->first] = MAXIMUM;
          // Previous node in optimal path from source
          //previous[it->first] := INFINITY;
          //std::pair<uint, uint> p(it->first, INFINITY);
     }
     distance[source] = 0;
     q.push(std::pair <uint, uint> (distance[source], source));
     while(!q.empty()) {
          node = q.top();  
          q.pop();
          uint id = node.second;
          if (visited.find(id) == visited.end()) {
               visited.insert(id);
               // Add all it's neighbors yet unvisited to the queue
               std::set<Edge>* adj;
               adj = getAdjacency(id);
               std::set<Edge>::iterator dest;
               for (dest = adj->begin(); dest != adj->end(); ++dest) {
                    if (visited.find(dest->getNode()) == visited.end() &&
                         distance[id] + dest->getWeight() < 
                         distance[dest->getNode()]) {
                         // If node not visited yet and the path to it
                         // is smaller then the current distance...
                         /*
                         std::cout << "distance[" << dest->getNode() <<
                              "] = " << distance[id] << " + " <<
                              dest->getWeight() << std::endl;
                         */
                         distance[dest->getNode()] = 
                              distance[id] + dest->getWeight();
                         q.push(std::pair <uint, uint> 
                              (distance[dest->getNode()], dest->getNode()));
                    }
               }
          }
     }
     return distance;
}

uint Graph::getNodeLabelId(std::string label) {
     return label_id[label];
}

bool Graph::nodeLabelExists(std::string label) {
     if (label_id.find(label) == label_id.end()) {
          //label does not exist
          return false;
     } else {
          return true;
     }
}
