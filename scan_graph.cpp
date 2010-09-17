#include "scan_graph.hpp"
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>

/********************************************************************
* Reads a graph from a gml file
*********************************************************************/
void ScanGraph::readGmlFile(std::string filename, bool self_loops) {
     // auxiliar vars
     std::string attr;
     std::string value;
     bool directed = false;
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
                    // fscanf(file, "%s %s", &attr, &value);
                    file >> attr;
                    while (attr != "]") {
                         // For now, the node id is the only attribute 
                         // we accept. We will skip anything else
                         if (attr == "id") {
                              file >> value;
                              nid = atoi(value.c_str());
                              graph_map[nid] = NULL;
                              graph_attributes[nid] = NULL;
                              // Setting the "unclassified" (-1) label 
                              // to all nodes
                              // label[nid] = -1;
                              // SCAN needs all nodes to have self loops, so...
                              // TODO the problem now is: what is the weight
                              // of a self loop? I never liked this, so maybe
                              // this is one more reason to remove it 
                              // altogether :D
                              if (self_loops) {
                                   addEdge(nid, nid);
                              } 
                              
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
                    // if (strcmp(attr, "[") != 0) {
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
* Reads a file with the following format:
*
* nodes (num_nodes) dir (1|0) weight (1|0)
* node_id
* ...
* edges
* node1 node2 (weight)
* ...
********************************************************************/
void ScanGraph::readFile(std::string filename, bool self_loops){
     // auxiliar vars
     std::string tmp, dir, weig;
     bool directed, weighted;
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
          for (uint i = 0; i < num_nodes; ++i) {
               file >> auxint;
               graph_map[auxint] = NULL;
               // Setting the "unclassified" (-1) label to all nodes
               // label[auxint] = -1;
               // SCAN needs all nodes to have self loops, so...
               // TODO the problem now is: what is the weight of a self loop?
               // I never liked this, so maybe this is one more reason to 
               // remove it altogether :D
               if (self_loops) {
                    addEdge(auxint, auxint);
               }

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
* Constructor
********************************************************************/
ScanGraph::ScanGraph() : Graph(){
}

/********************************************************************
* Constructor
********************************************************************/
ScanGraph::ScanGraph(std::string filename) : Graph(filename){
}

/********************************************************************
* Returns the value of a node's label (it's cluster)
********************************************************************/
//long ScanGraph::getLabel(uint node){
//     return label[node];
//}

/********************************************************************
* Sets the value of a node's label
********************************************************************/
//void ScanGraph::setLabel(uint node, long l){
//     label[node] = l;
//}

// Destructor
/*
ScanGraph::~ScanGraph() {
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
*/
