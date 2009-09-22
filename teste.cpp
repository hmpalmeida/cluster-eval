#include "scan.hpp"
#include <iostream>

int main() {
     //ScanGraph sg("data/astro-ph.gml");
     Scan s(1,"data/grafo-teste.txt");
     s.printGraph();
     std::cout << s.getEdgeWeight(4,2) << std::endl;
}
