#include "graph.hpp"

class Proximity {
     private:
          SquareMatrix<float> P;
          uint n;    // # nodes
          Proximity(); // Construtor default desabilitado
          // Construtor de cópia de atribuição?
     public:
          Proximity(Graph& g);
          ~Proximity();
          double getProximity(uint i, uint j);
          void printTransitionMatrix();
};
