#include "scan.hpp"
#include <exception> 
#include <string>
#include <iostream>
#include <signal.h>

float getBestEpsilon(float start, float end, float step, int mi, char* graph) {
     float best_mod, epsilon;
     best_mod = 0.0;
     for (float f = start; f <= end; f+=step) {
          Scan sc;
          try {
               sc.loadGraph(graph);
          } catch (std::string err) {
               std::cout << err << std::endl;
          }
          sc.run(f, mi);
          float mod = sc.getModularity();
          if (mod >= best_mod) {
               epsilon = f;
               best_mod = mod;
          }
     }
     return epsilon;
}

void runScan(float epsilon, int mi, char* graph) {
     Scan s;
     try {
          s.loadGraph(graph);
     } catch (std::string err) {
          std::cout << err << std::endl;
     }
     s.run(epsilon, mi);
     s.printAll();
}

int main(int argc, char** argv){
     if (argc < 2) {
          std::cout << "Too few arguments: " << argc -1 << std::endl;
          std::cout << "Possible uses:" << std::endl <<
               "\t ./scan r <epsilon> <mi> <graph_file>" << std::endl;
     } else {
          char x = (char)argv[1][0];
          switch (x) {
               // Run SCAN
               case 'r':
                    if (argc != 5) {
                         std:: cout << "Bad command." << std::endl;
                         std::cout << "Should be:" << std::endl <<
                         "\t ./scan r <epsilon> <mi> <graph_file>" << std::endl;
                         return 1;
                    }
                    runScan(atof(argv[2]), atoi(argv[3]), argv[4]);
                    break;
               // Find the best epsilon using modularity
               case 'e':
                    if (argc != 7) {
                         std:: cout << "Bad command." << std::endl;
                         std::cout << "Should be:" << std::endl <<
                         "\t ./scan e <Starting_e> <ending_e> <step> " <<
                         "<mi> <graph_file>" << std::endl;
                         return 1;
                    }
                    // FIXME Debugging
                    std::cout << "argv[6] = " << argv[6] << std::endl;
                    std::cout << "Best Epsilon: " << 
                         getBestEpsilon(atof(argv[2]),atof(argv[3]),
                                   atof(argv[4]),atoi(argv[5]),argv[6]) << 
                         std::endl;
                    break;
          }

     }
} 
