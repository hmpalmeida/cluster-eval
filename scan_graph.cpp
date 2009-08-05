#include "scan_graph.hpp"
#include <iostream>

void ScanGraph::readFile(std::string filename){
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
                    // Setting the "unclassified" (-1) label to all nodes
                    label[auxint] = -1;
                    // SCAN needs all nodes to have self loops, so...
                    addEdge(auxint, auxint);
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
          // Just so we know where we are
          std::cout << "Graph loaded" << std::endl;
     } else {
          // File did not open... report
          throw ("Error opening file.");
     }
}

ScanGraph::ScanGraph() : Graph(){
}

ScanGraph::ScanGraph(std::string filename) : Graph(filename){
}

long ScanGraph::getLabel(uint node){
     return label[node];
}

void ScanGraph::setLabel(uint node, long l){
     label[node] = l;
}
