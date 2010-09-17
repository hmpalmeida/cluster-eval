#include <iostream>
#include "proximity.hpp"

Proximity::Proximity(Graph& g) : P(g.getNumNodes()) {
     n = g.getNumNodes();
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

void Proximity::partialSymmetrization(float beta) {
     // First, let's mark all virtual edges that should be created
     // with a 9999.0
     for (int i = 0; i < n; ++i) {
          for (int j = 0; j < n; ++j) {
               if (P[i][j] > 0.0 && P[j][i] == 0.0) {
                    P[j][i] = 9999.0;
               }
          }
     }
     // Now we will fix the transition probabilities considering that
     // the virtual edges have beta probability
     for (int i = 0; i < n; ++i) {
          float prob_edges = 1.0;
          std::set<uint> edges;
          for (int j = 0; j < n; ++j) {
               if (P[i][j] > 0.0) {
                    if (P[i][j] == 9999.0) {
                         P[i][j] = beta;
                         prob_edges -= beta;
                    } else {
                         edges.insert(j);
                    }
               }
          }
          // Put the right transition values on the edges
          std::set<uint>::iterator setIt;
          for (setIt = edges.begin(); setIt != edges.end(); ++setIt) {
               P[i][*setIt] = prob_edges/(float)edges.size();
          }
     }

}

void Proximity::printTransitionMatrix() {
     for (int i = 0; i < n; ++i) {
          for (int j = 0; j < n; ++j) {
               std::cout << P[i][j] << "  ";
          }
          std::cout << std::endl;
     } 
}
