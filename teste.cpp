#include "graph.hpp"
#include <iostream>

int main() {
     Graph g;
     g.readFile("data/grafo-teste.txt");
     g.print();
}
