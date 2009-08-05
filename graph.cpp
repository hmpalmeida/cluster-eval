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

void Graph::readFile(std::string filename){
     // auxiliar vars
     char tmp[10];
     uint auxint, auxint2;
     FILE * file;
     
     num_edges = 0;

     // opening file
     file = fopen(filename.c_str(),"r");

     if (file != NULL) {
          // first line should start with "nodes" and the number of nodes
          fscanf(file, "%s %d", &tmp, &auxint);
          if (strcmp(tmp,"nodes") == 0) {
               num_nodes = auxint;
               for (int i = 0; i < num_nodes; ++i) {
                    fscanf(file, "%d", &auxint);
                    graph_map[auxint] = NULL;
               }
               // and now, the vertices
               fscanf(file,"%s",tmp);
               if (strcmp(tmp,"vertices") == 0) {
                    while (!feof(file)) {
                         fscanf(file,"%d %d",&auxint,&auxint2);
                         addEdge(auxint, auxint2);
                         ++num_edges;
                    }
               } else {
                    throw ("Input in wrong format: expecting \"vertices\"");
               }
          } else {
               throw ("Input in wrong format: expecting \"nodes\".");
          }
          fclose(file);
     } else {
          // File did not open... report
          throw ("Error opening file.");
     }
}

// Add an UNDIRECTED edge to the graph.
void Graph::addEdge(uint node1, uint node2){
     // adding reference to the first node
     if (graph_map[node1] == NULL) {
          // Set yet not created
          graph_map[node1] = new std::set<uint>;
     }
     // insert it. The set will handle possible duplicates
     graph_map[node1]->insert(node2);
     // adding reference to the second node
     if (graph_map[node2] == NULL) {
          // Set yet not created
          graph_map[node2] = new std::set<uint>;
     }
     // insert it. The set will handle possible duplicates
     graph_map[node2]->insert(node1);
}

// Destructor
Graph::~Graph() {
   hmap::const_iterator it;
   for (it = graph_map.begin(); it != graph_map.end(); ++it) {
      // Since the graph is connected, we can just delete all pointers
      // without fear of freeing any unallocated pointer
      delete it->second;
   }
}

// Print the vector. Mainly for testing purposes
void Graph::print() {
     hmap::iterator mapIt;
     std::set<uint>::iterator setIt;
     for (mapIt = graph_map.begin(); mapIt != graph_map.end(); ++mapIt) {
          std::cout << mapIt->first << " ->";
          for (setIt = mapIt->second->begin();
               setIt != mapIt->second->end();
               ++setIt) {
          std::cout << " " << *setIt;
          }
          std::cout << std::endl;
     }
}

std::set<uint>* Graph::getAdjacency(uint node){
     return graph_map[node];
}

uint Graph::getNumEdges() {
     return num_edges;
}
