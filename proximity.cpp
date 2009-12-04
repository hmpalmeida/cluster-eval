#include <iostream>
#include "proximity.hpp"

Proximity::Proximity(Graph& g) {
     n = g.getNumNodes();
     // Allocates the matrix
     SquareMatrix<float> matrix(n);
     P = matrix;
     // Now to create the transition matrix
     // First, zero the matrix
     for (int i = 0; i < n; ++i) {
          for (int j = 0; j < n; ++j) {
               P[i][j] = 0.0;
          }
     } 
     // Now, run through the graph setting the transition values
     // As it is, it will ignore any weights, counting only the number
     // of exiting edges
     hmap::iterator mapIt;
     std::set<Edge>::iterator setIt;
     for (mapIt = g.graph_map.begin(); mapIt != g.graph_map.end(); 
               ++mapIt) {
          if (mapIt->second != NULL) {
               for (setIt = mapIt->second->begin();
                    setIt != mapIt->second->end();
                    ++setIt) {
                    std::cout << "P[" << mapIt->first << "][" << 
                         setIt->getNode() << "] == 1/" << mapIt->second->size();
                    std::cout << " = " << 
                         1/(float)mapIt->second->size() << std::endl;
                    P[mapIt->first][setIt->getNode()] = 
                         1/(float)mapIt->second->size();
               }
          }
     }

}

Proximity::~Proximity() {
}

double Proximity::getProximity(uint i, uint j) {
}

void Proximity::printTransitionMatrix() {
     for (int i = 0; i < n; ++i) {
          for (int j = 0; j < n; ++j) {
               std::cout << P[i][j] << "  ";
          }
          std::cout << std::endl;
     } 
}
