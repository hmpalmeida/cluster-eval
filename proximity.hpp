#include "graph.hpp"

class Proximity {
     private:
          SquareMatrix<float> P;
          uint n;    // # nodes
          Proximity(); // Construtor default desabilitado
          // Creates virtual symmetric edges with beta probability
          void partialSymmetrization(float beta);
          // Construtor de cópia de atribuição?
     public:
          Proximity(Graph& g);
          ~Proximity();
          double getProximity(uint i, uint j);
          void printTransitionMatrix();
};
