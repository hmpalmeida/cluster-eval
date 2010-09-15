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
#include "intersection_naming.hpp"

/* 
 * Loads a file with clustering data for evaluation
 */
uint getClusters(char* fclusters, hmap_uint_suint* clusters, 
          std::vector<std::string>* id_clusters, Graph* g) {
     std::set<uint>* tmpset = NULL;
     // as always, no cluster 0
     id_clusters->push_back("");
     std::ifstream cluster_file;
     std::vector<std::string> tokens;
     std::string line;
     cluster_file.open(fclusters);
     uint field_counter, cid = 1;
     getline(cluster_file, line);
     // To count the nodes who appear in multiple clusters
     uint v_num_nodes = 0;
     std::set<uint> nodes;
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
          id_clusters->push_back(tokens[0]);
          cid = id_clusters->size()-1;
          if (tmpset == NULL) tmpset = new std::set<uint>;
          for (int i = 2; i < tokens.size(); ++i){
               // Node ids
               // FIXME O problema está aqui. Ele assume que os vertices  são
               // inteiros, o que não necessaiamente são. Precisamos montar
               // um registro de quais os ids para cada nó. Graph já tem isso?
               //tmpset->insert(atoi(tokens[i].c_str()));
               uint node_id;
               if (isNumber(tokens[i])) {
                    // May already be the node's id. Check
                    if (g->nodeLabelExists(tokens[i])) {
                         // Well, it IS a label. Who knew! Proceed as usual
                         node_id = g->getNodeLabelId(tokens[i]);
                    } else {
                         // Not a known label, so it is already a node id!
                         node_id = atoi(tokens[i].c_str());
                    }
               } else {
                    node_id = g->getNodeLabelId(tokens[i]);
               }
               tmpset->insert(node_id);
               if (nodes.find(node_id) != nodes.end()) {
                    ++v_num_nodes;
               } else {
                    nodes.insert(node_id);
               }
          }
          clusters->insert(std::pair<uint, std::set<uint>* >(cid, tmpset));
          tmpset = NULL;
          getline(cluster_file, line);
     }
     cluster_file.close();
     return v_num_nodes;
}

void getClustersByNode() {
}

void evaluate(char* fgraph, char* fclusters) {
     // Loading the graph data
     Graph gr(fgraph);
     // Loading the cluster data
     std::vector<std::string> id_clusters;
     hmap_uint_suint clusters;
     uint vnn = getClusters(fclusters, &clusters, &id_clusters, &gr);
     // Generating the clusters by node data
     hmap_uint_suint cluster_label;
     hmap_uint_suint::const_iterator it;
     std::set<uint>::iterator sit;
     for (it = clusters.begin(); it != clusters.end(); ++it) {
          for (sit = it->second->begin(); sit != it->second->end(); ++sit) {
               if (cluster_label[*sit] == NULL) 
                    cluster_label[*sit] = new std::set<uint>;
               cluster_label[*sit]->insert(it->first);
          }
          //std::cout << it->first << ": " << it->second->size() << std::endl;
     }
     // Adding cluster 0 to any node not in a cluster
     hmap::iterator hit;
     for (hit = gr.graph_map.begin(); hit != gr.graph_map.end(); ++hit) {
          if (cluster_label[hit->first] == NULL) {
               cluster_label[hit->first] = new std::set<uint>;
               cluster_label[hit->first]->insert(0);
          }
     }
     // Starting the cluster evaluator
     ClusterEvaluator ce(&gr, &clusters, &cluster_label, &id_clusters, vnn);
     std::vector<double> sil = ce.getSilhouetteIndex();
     for (int i = 1; i < sil.size(); ++i) {
          std::cout << "Si for cluster " << i << ": " << 
               sil[i] << std::endl;
     }
     float mod = ce.getModularity();
     std::cout << "Modularity is: " << mod << std::endl;

     float ent = ce.getGraphEntropy();
     std::cout << "Entropy is: " << ent << std::endl;
     
     // Coverage!
     double cov = ce.getCoverage();
     std::cout << "Coverage is: " << cov << std::endl;

     // Cleaning allocs
     for (it = clusters.begin(); it != clusters.end(); ++it) {
          delete it->second;
     }
     for (it = cluster_label.begin(); it != cluster_label.end(); ++it) {
          delete it->second;
     }
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
