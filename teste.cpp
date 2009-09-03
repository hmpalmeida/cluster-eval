#include "scan.hpp"
#include <iostream>

int main() {
     Scan s;
     try {
          s.loadGraph("data/grafo-teste.txt");
     } catch (std::string err) {
          std::cout << err << std::endl;
     }
     s.run(0.6, 2);
     s.writeAll();
}
