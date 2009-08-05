#include <iostream>
#include <fstream>
#include <cstring>
#include <ext/hash_set>

typedef __gnu_cxx::hash_set<unsigned int> hset;

int main (int argc, char** argv) {
     std::ifstream infile_outs;
     infile_outs.open("outs.txt");
     unsigned int node;
     hset outs;
     // Loading all outliers
     while (!infile_outs.eof()){
          infile_outs >> node;
          outs.insert(node);
     }
     infile_outs.close();
     
     // Now remove them from the graph
     // opening file
     FILE * file;
     char tmp[10];

     file = fopen("undir-no-weight.txt","r");
     unsigned int auxint, auxint2, num_nodes;
     if (file != NULL) {
          // first line should start with "nodes" and the number of nodes
          fscanf(file, "%s %d", &tmp, &auxint);
          if (strcmp(tmp,"nodes") == 0) {
               num_nodes = auxint;
               std::cout << "nodes " << num_nodes << std::endl;
               for (int i = 0; i < num_nodes; ++i) {
                    fscanf(file, "%d", &auxint);
                    // If it is not an outlier
                    if (outs.find(auxint) == outs.end()) {
                         std::cout << auxint << std::endl;
                    }
               }
               // and now, the vertices
               fscanf(file,"%s",tmp);
               if (strcmp(tmp,"vertices") == 0) {
                    std::cout << "vertices" << std::endl;
                    while (!feof(file)) {
                         fscanf(file,"%d %d",&auxint,&auxint2);
                         if (outs.find(auxint) == outs.end() &&
                             outs.find(auxint2) == outs.end()) {
                              std::cout << auxint << " " << auxint2 
                                   << std::endl;
                         }
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
