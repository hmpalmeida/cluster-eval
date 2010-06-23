#include "scan.hpp"
#include <exception> 
#include <string>
#include <iostream>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include "cluster_evaluator.hpp"

float getBestEpsilon(float start, float end, float step, int mi, 
          uint simi_type, char* graph) {
     float best_mod, epsilon;
     best_mod = 0.0;
     for (float f = start; f <= end; f+=step) {
          Scan *sc = new Scan();
          try {
               sc->loadGraph(graph);
          } catch (std::string err) {
               std::cout << err << std::endl;
          }
          sc->setSimFunction(simi_type);
          sc->run(f, mi);
          std::cout << "Modularity calculus... ";
          ClusterEvaluator ce(&sc->g, &sc->clusters, &sc->cluster_label);
          /*
          float mod = ce.getModularity();
          std::cout << "DONE!" << std::endl;
          std::cout << "e = " << f << "\t mod = " << mod << std::endl;
          if (mod >= best_mod) {
               epsilon = f;
               best_mod = mod;
          }*/
          ce.getSilhouetteIndex();
          delete sc;
     }
     return epsilon;
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
               "\t ./scan r <epsilon> <mi> <sim_function_#> <graph_file>" <<
               std::endl;
          std::cout << "\t ./scan e <Starting_e> <ending_e> <step> " <<
               "<mi> <sim_function_#> <graph_file>" << std::endl;
     } else {
          char x = (char)argv[1][0];
          switch (x) {
               // Run SCAN
               case 'r':
                    if (argc != 6) {
                         std:: cout << "Bad command." << std::endl;
                         std::cout << "Should be:" << std::endl <<
                         "\t ./scan r <epsilon> <mi>  <sim_function_#> <graph_file>" << std::endl;
                         return 1;
                    }
                    runScan(atof(argv[2]), atoi(argv[3]), atoi(argv[4]), 
                              argv[5]);
                    break;
               // Find the best epsilon using modularity
               case 'e':
                    if (argc != 8) {
                         std:: cout << "Bad command." << std::endl;
                         std::cout << "Should be:" << std::endl <<
                         "\t ./scan e <Starting_e> <ending_e> <step> " <<
                         "<mi> <sim_function_#> <graph_file>" << std::endl;
                         return 1;
                    }
                    std::cout << "Best Epsilon: " << 
                         getBestEpsilon(atof(argv[2]),atof(argv[3]),
                                   atof(argv[4]),atoi(argv[5]),atoi(argv[6]),
                                   argv[7]) << std::endl;
                    break;
          }

     }
} 
