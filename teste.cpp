//#include "proximity.hpp"
//#include "utils.hpp"
#include "intersection_naming.hpp"
#include "multiple_naming.hpp"
//#include "ocurrence.hpp"
//#include "utils.hpp"
#include <iostream>
#include <string>
#include <set>

int main() {
     //NamingGame ng("data/dblp-lcc.gml");
     //MultipleNamingGame ng("data/dblp-lcc.gml", 50, 3);
     //IntersectionNamingGame ng("data/marvel-graph-50n-nosheref.gml");
     //IntersectionNamingGame ng("data/teste.gml");
     IntersectionNamingGame ng("data/dblp-lcc.gml");
     //ng.loadRandomWord(40);
     //ng.g.printDotFile("marvel50n.dot");
     ng.run(10);
     //ng.printResults();
     //std::cout << "****************" << std::endl;
     //ng.printResultsByLabel();
     //std::cout << "Merging labels!" << std::endl;
     //std::cout << "------------------------------------" << std::endl;
     ng.mergeLabels2(0.5);
     ng.printResultsByLabel();
     //ng.printDotClusters("marvel50nclusters.dot");
     //ng.g.printClutoFile("marvel50n.cluto");
     //Graph g("data/teste.txt");
     //Proximity pr(g);
     //pr.printTransitionMatrix();
     //Scan s(1,"data/astro-ph.gml");
     //s.g.printWeights();
     //std::cout << s.getEdgeWeight(4,2) << std::endl;
}
