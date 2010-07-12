//#include "utils.hpp"
#include "graph.hpp"
#include <string>
#include <vector>

class NamingGame {
     protected:
          std::vector<uint> node_ids;
          uint chooseNode();
          int chooseNeighbor(uint node);
          virtual std::string chooseWord(uint node);
          bool communicate(uint sender, uint receiver, std::string word);
          std::tr1::unordered_map<std::string, std::set<uint> > clusters;
     public:
          Graph g;
          NamingGame();
          NamingGame(std::string filename);
          ~NamingGame();
          void loadGraph(std::string filename);
          virtual void run(uint rounds);
          void printResults();
          virtual void printResultsByLabel();
          void loadRandomWord(uint size);
          void printDotClusters(std::string filename, uint thold = 4);
};
