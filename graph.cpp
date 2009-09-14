#include "graph.hpp"
#include <iostream>
#include <exception>
#include <stdio.h>
#include <fstream>
#include <math.h>

/********************************************************************
* Just an empty constructor
********************************************************************/
Graph::Graph(){
     greatest_weight = -999999.0;
     smallest_weight = 999999.0;
}

/********************************************************************
* Constructor that automatically loads the graph from a file
********************************************************************/
Graph::Graph(std::string filename){
     greatest_weight = -9999.0;
     smallest_weight = 9999.0;
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
     bool directed = false;
     std::ifstream file;
     double weight;
     int braces = 0, source, target;
     
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
                    while (attr != "]") {
                         // For now, the node id is the only attribute 
                         // we accept. We will skip anything else
                         if (attr == "id") {
                              file >> value;
                              graph_map[atoi(value.c_str())] = NULL;
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
     char tmp[10], dir[5], weig[10], directed, weighted;
     uint auxint, auxint2;
     FILE * file;
     double auxdouble;
     
     num_edges = 0;

     // opening file
     file = fopen(filename.c_str(),"r");

     if (file != NULL) {
          // first line should start with "nodes" and the number of nodes
          // tests.google.com (parte do code.google.com, o nome pode 
          // não ser esse)
          fscanf(file, "%s %d %s %c %s %c", &tmp, &auxint, &dir, &directed,
                                             &weig, &weighted);
          // Checking the file format
          if ((strcmp(tmp,"nodes")   != 0) ||
              (strcmp(dir,"dir")     != 0) ||
              (strcmp(weig,"weight") != 0)) {
               throw ("Input in wrong format: expecting \"nodes (num_nodes) dir (1|0) weight (1|0) \".");
          }
          // Everything ok? Proceed reading the nodes
          num_nodes = auxint;
          for (int i = 0; i < num_nodes; ++i) {
               fscanf(file, "%d", &auxint);
               graph_map[auxint] = NULL;
          }
          // and now, the edges
          fscanf(file,"%s",tmp);
          if (strcmp(tmp,"edges") != 0) {
               throw ("Input in wrong format: expecting \"vertices\"");
          }
          // Fugly, I know, but gets the job done
          if ((weighted == '0') && (directed == '0')) {
               while (!feof(file)) {
                    fscanf(file,"%d %d",&auxint,&auxint2);
                    addEdge(auxint, auxint2);
                    addEdge(auxint2, auxint);
                    ++num_edges;
               }
          } else if ((weighted == '0') && (directed == '1')) {
               while (!feof(file)) {
                    fscanf(file,"%d %d",&auxint,&auxint2);
                    addEdge(auxint, auxint2);
                    ++num_edges;
               }
          } else if ((weighted == '1') && (directed == '0')) {
               while (!feof(file)) {
                    fscanf(file,"%d %d %f",&auxint,&auxint2, &auxdouble);
                    addEdge(auxint, auxint2, auxdouble);
                    addEdge(auxint2, auxint, auxdouble);
                    ++num_edges;
               }
          } else {
               while (!feof(file)) {
                    fscanf(file,"%d %d %f",&auxint,&auxint2, &auxdouble);
                    addEdge(auxint, auxint2, auxdouble);
                    ++num_edges;
               }
          }
          fclose(file);
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
* Normalizes the edge weights, if they exist
********************************************************************/
void Graph::normalizeWeights() {
     // If the graph is weighted
     if ((greatest_weight != 0) && (smallest_weight != 0)) {
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
