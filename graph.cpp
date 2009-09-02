#include "graph.hpp"
#include <iostream>
#include <exception>
#include <stdio.h>

// just an empty constructor
Graph::Graph(){
}

Graph::Graph(std::string filename){
     readFile(filename);
}


// Expects a file formatted like this::
// nodes (num_nodes) dir (1|0) weight (1|0)
// node_id
// ...
// edges
// node1 node2 (weight)
// ...
void Graph::readFile(std::string filename){
     // auxiliar vars
     char tmp[10], dir[5], weig[10], directed, weighted;
     uint auxint, auxint2;
     FILE * file;
     float auxfloat;
     
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
                    fscanf(file,"%d %d %f",&auxint,&auxint2, &auxfloat);
                    addEdge(auxint, auxint2, auxfloat);
                    addEdge(auxint2, auxint, auxfloat);
                    ++num_edges;
               }
          } else {
               while (!feof(file)) {
                    fscanf(file,"%d %d %f",&auxint,&auxint2, &auxfloat);
                    addEdge(auxint, auxint2, auxfloat);
                    ++num_edges;
               }
          }
          fclose(file);
     } else {
          // File did not open... report
          throw ("Error opening file.");
     }
}


// is necessary, add it both ways. Also, needs to use the "Edge" class
// Add a DIRECTED edge to the graph. Must Add both ways if undirected
void Graph::addEdge(uint node1, uint node2, float weight){
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
}

// Destructor
Graph::~Graph() {
     hmap::const_iterator it;
     for (it = graph_map.begin(); it != graph_map.end(); ++it) {
          // Now, Delete the set
          delete it->second;
     }
}

// Print the vector. Mainly for testing purposes
void Graph::print() {
     hmap::iterator mapIt;
     std::set<Edge>::iterator setIt;
     for (mapIt = graph_map.begin(); mapIt != graph_map.end(); ++mapIt) {
          std::cout << mapIt->first << " ->";
          for (setIt = mapIt->second->begin();
               setIt != mapIt->second->end();
               ++setIt) {
               std::cout << " " << setIt->toString();
          }
          std::cout << std::endl;
     }
}

std::set<Edge>* Graph::getAdjacency(uint node){
     return graph_map[node];
}

uint Graph::getNumEdges() {
     return num_edges;
}
