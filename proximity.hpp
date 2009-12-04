#include "graph.hpp"

class Proximity {
     private:
          float** P; // the transition matrix
          uint n;    // # nodes
     public:
          Proximity(Graph& g);
          ~Proximity();
          double getProximity(uint i, uint j);
          void printTransitionMatrix();
};
