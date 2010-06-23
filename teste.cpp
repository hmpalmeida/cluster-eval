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
     //NamingGame ng("data/football.gml");
     //MultipleNamingGame ng("data/marvel-graph-20t-nosheref.gml", 30, 1);
     IntersectionNamingGame ng("data/marvel-graph-50n-nosheref.gml");
     //IntersectionNamingGame ng("data/teste.gml");
     //IntersectionNamingGame ng("data/marvel-graph-50n.gml");
     //ng.loadRandomWord(15);
     //ng.g.printDotFile("marvel50n.dot");
     //ng.run(10);
     //ng.printResults();
     //std::cout << "****************" << std::endl;
     //ng.printResultsByLabel();
     //ng.printDotClusters("marvel50nclusters.dot");
     ng.g.printClutoFile("marvel50n.cluto");
     //Graph g("data/teste.txt");
     //Proximity pr(g);
     //pr.printTransitionMatrix();
     //Scan s(1,"data/astro-ph.gml");
     //s.g.printWeights();
     //std::cout << s.getEdgeWeight(4,2) << std::endl;
}
