#include "scan_graph.hpp"
#include <iostream>

int main() {
     ScanGraph sg;
     sg.readFile("data/grafo-teste.txt");
     sg.print();
}
