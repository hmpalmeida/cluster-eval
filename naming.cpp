#include "naming.hpp"
#include <iostream>
#include <set>
#include <iterator>
#include <time.h>
#include <stdlib.h>
#include <fstream>

/********************************************************************
* Constructor
********************************************************************/
NamingGame::NamingGame(){
}

/********************************************************************
* Other constructor
********************************************************************/
NamingGame::NamingGame(std::string filename){
     loadGraph(filename);
}

/********************************************************************
* Destructor TODO Don't know what to destroy... yet
********************************************************************/
NamingGame::~NamingGame() {
     /*
     hmap_uint_suint::const_iterator it;
     for (it = clusters.begin(); it != clusters.end(); ++it) {
          delete it->second;
     }
          for (it = hubs.begin(); it != hubs.end(); ++it) {
          delete it->second;
     }
     */
} 

/********************************************************************
* Loads a graph. Will decide which method should be used
********************************************************************/
void NamingGame::loadGraph(std::string filename){
     if (filename.compare(filename.size()-4,4,".gml") == 0) {
          try {
               g.readGmlFile(filename);
          } catch (std::string err) {
               std::cout << err << std::endl;
          }
     } else {
          try {
               g.readFile(filename);
          } catch (std::string err) {
               std::cout << err << std::endl;
          }
     }
     // Load the auxilary <<node_ids> vector. It is needed because we do not
     // assume consecutive node ids and need fast choosing of random nodes,
     // which the hash map cannot do...
     node_ids.reserve(g.graph_map.size());
     hmap::iterator it;
     for (it = g.graph_map.begin(); it != g.graph_map.end(); ++it) {
          node_ids.push_back(it->first);     
     }
}

/********************************************************************
* Runs the naming game for a maximum of <rounds> rounds
********************************************************************/
void NamingGame::run(uint rounds) {
     uint sender, receiver;
     srand(time(NULL));
     std::string word;
     // For am maximum of <rounds> rounds
     for (int i = 0; i < rounds; ++i) {    
          // Choose one random node (sender)
          sender = chooseNode();
          // Choose one random neighbor (receiver)
          receiver = chooseNeighbor(sender);
          if (receiver == -1) {
               --i;
               continue;
          }
          // Choose one random word from the sender;
          word = chooseWord(sender);
          // Send him an random word. Hijinks ensue
          communicate(sender, receiver, word);
          // FIXME Testing only
          //printResults();
          //if (i % 100 == 0) std::cout << "Round " << i << std::endl;
     }
     // ??? (TODO)
     // Profit!  
}

/********************************************************************
* Returns a randomly chosen node
********************************************************************/
uint NamingGame::chooseNode() {
     uint index;
     //index =  rand() % node_ids.size();
     index =  randomNumber(node_ids.size());
     return node_ids[index];
}

/********************************************************************
* Randomly returns one of <node>'s neighbors  
********************************************************************/
int NamingGame::chooseNeighbor(uint node) {
     // Since the number of neighbors of a single node is (probably) much
     // smaller then the whole graph, we will iterate on demand...
     // If run times get problematic, we will have to switch the whole
     // graph structure to consider sequential ids and use vectors to
     // map nodes and neighborhoods.
     
     // Get <node>'s adjacency list     
     std::set<Edge> *adj;
     adj = g.getAdjacency(node);
     if (adj == NULL) return -1; 
     // Get a random index value for a neighbor
     uint index;
     //index =  rand() % adj->size();
     index =  randomNumber(adj->size());
     // Iterate the set to find the chosen neighbor
     std::set<Edge>::iterator it = adj->begin();
     std::advance(it, index);
     return it->getNode();
}

/********************************************************************
* Returns a random word from <node>'s vocabulary  
********************************************************************/
std::string NamingGame::chooseWord(uint node) {
     std::set<std::string> *voc = g.getVocabulary(node);
     // Get a random index value for a word
     uint index;
     //index =  rand() % voc->size();
     index =  randomNumber(voc->size());
     // Iterate the set to find the chosen neighbor
     std::set<std::string>::iterator it = voc->begin();
     std::advance(it, index);
     return *it;
}

/********************************************************************
* Does the effective communication between two nodes
********************************************************************/
bool NamingGame::communicate(uint sender, uint receiver, std::string word) {
     //std::cout << "NamingGame communicate!" << std::endl;     
     if (g.hasWord(receiver, word)) {
          // Communication success
          g.dropVocabulary(sender);
          g.dropVocabulary(receiver);
          g.addWord(sender, word);
          g.addWord(receiver, word);
          return true;
     } else {
          // Comunication failure
          g.addWord(receiver, word);
          return false;
     }
}

/********************************************************************
* Will load each node with a random word. Will erase any word it might have known 
********************************************************************/
void NamingGame::loadRandomWord(uint size) {
     srand(time(NULL));
     hmap_i_ss::iterator it;
     for (it = g.graph_attributes.begin(); 
             it != g.graph_attributes.end(); ++it) {
          if (it->second == NULL) it->second = new std::set<std::string>;
          it->second->clear();
          it->second->insert(rand_alnum_str(size));
     }
}

/********************************************************************
* Prints each node and the words it knows 
********************************************************************/
void NamingGame::printResults() {
     g.printVocabularies();
}

/********************************************************************
* Prints each node and the words it knows 
********************************************************************/
void NamingGame::printResultsByLabel() {
     std::tr1::unordered_map<std::string, std::set<uint> > clusters;
     hmap_i_ss::iterator it;
     std::set<std::string>::iterator sit;
     uint nid = 0;
     // First gather all nodes by label
     for (it = g.graph_attributes.begin(); 
             it != g.graph_attributes.end(); ++it) {
          nid = it->first;
          for (sit = it->second->begin(); sit != it->second->end(); ++sit) {
               clusters[*sit].insert(nid);
          }
     }
     // Now print them!
     std::tr1::unordered_map<std::string, std::set<uint> >::iterator cit;
     std::set<uint>::iterator siit;
     for (cit = clusters.begin(); cit != clusters.end(); ++cit) {
          std::cout << cit->first << " -> ";
          for (siit = cit->second.begin(); siit != cit->second.end();
                    ++siit) {
               std::cout << " , " << g.getNodeLabel(*siit);
          }
          std::cout << std::endl;
     }
}

void NamingGame::printDotClusters(std::string filename, uint thold) {
     std::fstream file(filename.c_str(), std::ios::out);
     std::string edge(" -- ");
     (g.isDirected()) ? edge = " -> " : edge = " -- ";
     hmap::iterator mapIt;
     std::set<Edge>::iterator setIt;
     filename.erase(filename.size() - 4);
     file << "graph " << filename << " { " << std::endl;
     // First the nodes and it's labels
     for (mapIt = g.graph_map.begin(); mapIt != g.graph_map.end(); ++mapIt) {
          file << "\t" << mapIt->first << " [label = \"" << 
               g.getNodeLabel(mapIt->first) << "\"];" << std::endl; 
     }
     // Now the clusters (TODO)

     std::tr1::unordered_map<std::string, std::set<uint> > clusters;
     hmap_i_ss::iterator it;
     std::set<std::string>::iterator sit;
     uint nid = 0;
     // First gather all nodes by label
     for (it = g.graph_attributes.begin(); 
             it != g.graph_attributes.end(); ++it) {
          nid = it->first;
          for (sit = it->second->begin(); sit != it->second->end(); ++sit) {
               clusters[*sit].insert(nid);
          }
     }
     // Now print them!
     std::tr1::unordered_map<std::string, std::set<uint> >::iterator cit;
     std::set<uint>::iterator siit;
     for (cit = clusters.begin(); cit != clusters.end(); ++cit) {
          //subgraph "" {label = ""; node1; node2;}
          if (cit->second.size() >= thold) {
               file << "\tsubgraph cluster_";
               file << cit->first << " {label = \"" << cit->first << "\"; ";
               for (siit = cit->second.begin(); siit != cit->second.end();
                         ++siit) {
                    file << *siit << "; ";
               }
               file << "}" << std::endl;
          }
     }



     // Now the edges
     if (g.isDirected()) {
          for (mapIt = g.graph_map.begin(); mapIt != g.graph_map.end(); 
                    ++mapIt) {
               if (mapIt->second != NULL) {
                    for (setIt = mapIt->second->begin();
                              setIt != mapIt->second->end();
                              ++setIt) {
                         file << "\t" << mapIt->first << edge << 
                         setIt->getNode() << ";" << std::endl;
                    }
               }
          }
     } else {
          std::string tmp;
          std::set<std::string> tmp_set;
          for (mapIt = g.graph_map.begin(); mapIt != g.graph_map.end(); 
                    ++mapIt) {
               if (mapIt->second != NULL) {
                    for (setIt = mapIt->second->begin();
                         setIt != mapIt->second->end();
                         ++setIt) {
                         if (mapIt->first < setIt->getNode()) {
                         tmp = "\t" + to_string<uint>(mapIt->first) + edge + 
                              to_string<uint>(setIt->getNode()) + ";";
                         } else {
                         tmp = "\t" + to_string<uint>(setIt->getNode()) + edge
                              + to_string<uint>(mapIt->first) + ";";
                         }
                         tmp_set.insert(tmp);
                    }
               }
          }
          std::set<std::string>::iterator ssit;
          for (ssit = tmp_set.begin(); ssit != tmp_set.end(); ++ssit) {
               file << *ssit << std::endl;
          }
     }
     file << "}" << std::endl;
     file.close();

}
