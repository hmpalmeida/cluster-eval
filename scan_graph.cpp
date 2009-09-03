#include "scan_graph.hpp"
#include <iostream>

// Expects a file formatted like this::
// nodes (num_nodes) dir (1|0) weight (1|0)
// node_id
// ...
// edges
// node1 node2 (weight)
// ...
void ScanGraph::readFile(std::string filename){
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
               // Setting the "unclassified" (-1) label to all nodes
               label[auxint] = -1;
               // SCAN needs all nodes to have self loops, so...
               // TODO the problem now is: what is the weight of a self loop?
               // I never liked this, so maybe this is one more reason to 
               // remove it altogether :D
               addEdge(auxint, auxint);

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
