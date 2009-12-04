#include "proximity.hpp"
#include <iostream>

int main() {
     Graph g("data/teste.txt");
     Proximity pr(g);
     pr.printTransitionMatrix();
     //Scan s(1,"data/astro-ph.gml");
     //s.g.printWeights();
     //std::cout << s.getEdgeWeight(4,2) << std::endl;
}
