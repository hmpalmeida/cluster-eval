#include "scan.hpp"
#include <exception> 
#include <string>
#include <iostream>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fstream>
#include "cluster_evaluator.hpp"

/* FIXME FIXME FIXME!!!!!!!!!
 * Clusters assumes the cluster id will be an int, and that is not right.
 * Prepare accordingly!
 */


// TODO Terminar com o carregamento dos parâmetros. Ainda tenho que adaptar o
// silhouette index para tratar o caso de overlapping!
void getClusters(char* fclusters, hmap_uint_suint* clusters) {
     std::set<uint>* tmpset = NULL;
     std::ifstream cluster_file;
     std::vector<std::string> tokens;
     std::string line;
     cluster_file.open(fclusters);
     uint field_counter, cid = 1;
     getline(cluster_file, line);
     while (!cluster_file.eof()) {
          // parse it!
          tokens = stringTokenizer(line);
          field_counter = 0;
          if (tokens.size() < 3) {
               std::cout << "Error! Too few elements in line.\n";
               exit(1);
          }
          if (tokens[1] != "->") {
               std::cout << "Error! Wrong format!.\n";
               exit(1);
          }
          // Cluster id
          cid = atoi(tokens[0].c_str());
          if (tmpset == NULL) tmpset = new std::set<uint>;
          for (int i = 2; i < tokens.size(); ++i){
               // Node ids
               tmpset->insert(atoi(tokens[i].c_str()));
          }
          clusters->insert(std::pair<uint, std::set<uint>* >(cid, tmpset));
          tmpset = NULL;
          getline(cluster_file, line);
     }
     cluster_file.close();
}

void getClustersByNode() {
}

void evaluate(char* fgraph, char* fclusters) {
     // Loading the graph data
     Graph gr(fgraph);
     // Loading the cluster data
     hmap_uint_suint clusters;
     getClusters(fclusters, &clusters);
     // Generating the clusters by node data
     // FIXME Adaptar para que o clusters by label seja um hmap_uint_suint
     // também. CEval vai ter que mudar!
     hmap_uint_suint cluster_label;
     hmap_uint_suint::const_iterator it;
     for (it = clusters.begin(); it != clusters.end(); ++it) {
          std::cout << it->first << ": " << it->second->size() << std::endl;
     }
     // Starting the cluster evaluator
     ClusterEvaluator ce(&gr, &clusters, &cluster_label);
     
     // Cleaning allocs
     for (it = clusters.begin(); it != clusters.end(); ++it) {
          delete it->second;
     }

     /*
          std::cout << "Modularity calculus... ";
          ClusterEvaluator ce(&sc->g, &sc->clusters, &sc->cluster_label);
          
          //float mod = ce.getModularity();
          //std::cout << "DONE!" << std::endl;
          //std::cout << "e = " << f << "\t mod = " << mod << std::endl;
          //if (mod >= best_mod) {
          //     epsilon = f;
          //     best_mod = mod;
          //}
          std::vector<double> sil = ce.getSilhouetteIndex();
          for (int i = 1; i < sil.size(); ++i) {
               std::cout << "Si for cluster " << i << ": " << 
                    sil[i] << std::endl;
          }
          std::cout << "Si for the whole graph: " << sil[0] << std::endl;
          delete sc;
     //}
     //return epsilon;
     */
}

void runScan(float epsilon, int mi, uint simi_type, char* graph) {
     Scan *s = new Scan(simi_type);
     try {
          s->loadGraph(graph);
     } catch (std::string err) {
          std::cout << err << std::endl;
     }
     //s->printGraph();
     s->run(epsilon, mi);
     s->writeAll(epsilon, mi);
     delete s;
}

int main(int argc, char** argv){
     if (argc < 2) {
          std::cout << "Too few arguments: " << argc -1 << std::endl;
          std::cout << "Possible uses:" << std::endl <<
             "\t ./cluster scan <epsilon> <mi> <sim_function_#> <graph_file>"
               << std::endl;
          std::cout << "\t ./cluster eval <graph_file> <cluster_file>" 
               << std::endl;
     } else {
          std::string x(argv[1]);
          if (x == "scan") {
               // Run SCAN
               if (argc != 6) {
                    std:: cout << "Bad command." << std::endl;
                    std::cout << "Should be:" << std::endl <<
                    "\t ./cluster scan <epsilon> <mi>  <sim_function_#> <graph_file>" << std::endl;
                    return 1;
               }
               runScan(atof(argv[2]), atoi(argv[3]), atoi(argv[4]), 
                         argv[5]);
          } else if (x == "eval") {
               // Find the best epsilon using modularity
               if (argc != 4) {
                    std:: cout << "Bad command." << std::endl;
                    std::cout << "Should be:" << std::endl <<
                    "\t ./cluster eval <graph_file> <cluster_file>" << 
                    std::endl;
                    return 1;
               }
               //std::cout << "Best Epsilon: " << 
               evaluate(argv[2], argv[3]);
               //               atof(argv[4]),atoi(argv[5]),atoi(argv[6]),
               //               argv[7]) << std::endl;
          }
     }
} 
