#include "multiple_naming.hpp"
#include <vector>
#include <algorithm>
#include <queue>
#include <set>

typedef std::tr1::unordered_map<uint, std::set<uint>* > hmap_uint_suint;


MultipleNamingGame::MultipleNamingGame() {
}

MultipleNamingGame::MultipleNamingGame(std::string filename, uint max_voc, 
          uint drops) {
     loadGraph(filename);
     num_drops = drops;
     max_voc_size = max_voc;
     hmap::iterator it;
     std::set<std::string>::iterator sit;
     std::set<std::string> *voc;
     Ocurrence oc("x");
     for (it = g.graph_map.begin(); it != g.graph_map.end(); ++it) {
          voc = g.getVocabulary(it->first);
          voc_xp[it->first] = new std::set<Ocurrence>;
          // Iterate the set to fill the expanded vocabulary
          for (sit = voc->begin(); sit != voc->end(); ++sit) {
               //Ocurrence oc(*sit);
               oc.setWord(*sit);
               //std::cout << "oc = " << oc.getPrintable() << std::endl;
               //addOcurrenceXP(it->first, oc);
               voc_xp[it->first]->insert(oc);
               //std::cout << "Added word " << *sit << " to " << it->first <<
               //     ". Voc Size: " << voc_xp[it->first]->size() << std::endl;
          }
     }
     std::set<Ocurrence>::iterator setit;
     //std::cout << "Vocabulary from node 1: ";
     //Ocurrence a;
     //for (setit = voc_xp[1]->begin(); setit != voc_xp[1]->end(); ++setit) {
     //     a = *setit;
     //     std::cout << " , " << a.getWord();
     //}
     //std::cout << std::endl;
}

MultipleNamingGame::~MultipleNamingGame() {
     hmap::iterator it;
     for (it = g.graph_map.begin(); it != g.graph_map.end(); ++it) {
          delete voc_xp[it->first];
     }
}


/*
 * Instead of choosing a word from the <graph>'s vocabulary, use
 * the Multiple Naming Game's one
 */
std::string MultipleNamingGame::chooseWord(uint node) {
     //std::cout << "MultipleNamingGame::chooseWord" << std::endl;
     std::set<Ocurrence> *voc = getVocabularyXP(node);
     if (voc->size() == 0) return "";
     // Get a random index value for a word
     uint index;
     //index =  rand() % voc->size();
     index =  randomNumber(voc->size());
     // Iterate the set to find the chosen neighbor
     std::set<Ocurrence>::iterator it = voc->begin();
     std::advance(it, index);
     Ocurrence o = *it;
     return o.getWord();
}


bool MultipleNamingGame::communicate(uint sender, uint receiver, 
          std::string word) {
     // TODO fazer a nova política de comunicação!
     //std::cout << "MultipleNamingGame communicate!" << std::endl;
     Ocurrence rec_o;
     rec_o = popWordXP(receiver, word);
     if (rec_o.getWord() != "") {
          // Communication success. Now what?
          // Remove <o> from the sender too
          Ocurrence send_o;
          // Because the sender and receiver might have different <count>s
          // for the same word
          send_o = popWordXP(sender, word);
          // Drop some of the vocabulary
          dropVocabularyXP(sender, num_drops);
          dropVocabularyXP(receiver, num_drops);
          // Add the updated <count> <o>
          addOcurrenceXP(sender, send_o);
          addOcurrenceXP(receiver, rec_o);
          return true;
     } else {
          // Communication failure. Now what?
          // Add <word> to the <receiver>'s vocabulary
          Ocurrence new_o(word);
          addOcurrenceXP(sender, new_o);
          addOcurrenceXP(receiver, new_o);
          return false;
     }
}


std::set<Ocurrence>* MultipleNamingGame::getVocabularyXP(uint node) {
     return voc_xp[node];
}

Ocurrence MultipleNamingGame::popWordXP(uint node, std::string word) {
     std::set<Ocurrence>::iterator it;
     Ocurrence o(word);
     it = voc_xp[node]->find(o);
     if (it == voc_xp[node]->end()) {
          // <node> does not know about the bird
          Ocurrence empty_o;
          return empty_o;
     } else {
          // <node> knows that the bird is the word
          // Upgrade the word rating
          o = *it;
          o.addCounter();
          voc_xp[node]->erase(it);
          return o;
     }
}

void MultipleNamingGame::dropVocabularyXP(uint node, uint num) {
     uint count = 0;
     // Put the set in a Vector and sort it to be able to remove 
     // the oldest stuff
     std::vector<Ocurrence> vec(voc_xp[node]->begin(), voc_xp[node]->end());
     std::sort(vec.begin(), vec.end(), decreasingSortOcurrence);
     // TODO Keeping one element always. Should I?
     while (voc_xp[node]->size() > 0 && count < num_drops) {
          // Remove the worse ranked ocurrence
          voc_xp[node]->erase(vec[count]);
          ++count;
     }
}

void MultipleNamingGame::addOcurrenceXP(uint node, Ocurrence o) {
     if (voc_xp[node]->size() >= max_voc_size) {
          dropVocabularyXP(node, 1);
     }
     voc_xp[node]->insert(o);
}


/********************************************************************
* Runs the naming game for a maximum of <rounds> rounds
********************************************************************/
void MultipleNamingGame::run(uint rounds) {
     uint sender, receiver;
     srand(time(NULL));
     std::string word;
     // FIXME TESTING!!!!
     /*
     hmap::iterator it;
     uint node  = 0;
     for (it = g.graph_map.begin(); it != g.graph_map.end(); ++it) {
          node = it->first;
          if (g.getNodeLabel(node) == "spider-man") {
               std::cout << g.getNodeLabel(node) << ": " << 
                    g.graph_map[node]->size() << std::endl;
          }
     }*/
     /*
     hmap::iterator it;
     uint node  = 0;
     for (it = g.graph_map.begin(); it != g.graph_map.end(); ++it) {
          node = it->first;
          std::cout << g.getNodeLabel(node) << ": " << chooseWord(node)
               << std::endl;
     }
     */

     // For am maximum of <rounds> rounds
     for (int i = 0; i < rounds; ++i) {    
          // Choose one random node (sender)
          sender = NamingGame::chooseNode();
          // Choose one random neighbor (receiver)
          receiver = NamingGame::chooseNeighbor(sender);
          if (receiver == -1) {
               --i;
               continue;
          }
          // Choose one random word from the sender;
          word = MultipleNamingGame::chooseWord(sender);
          if (word == "") {
               --i;
               continue;
          }
          // Send him an random word. Hijinks ensue
          MultipleNamingGame::communicate(sender, receiver, word);
          // FIXME Testing only
          //printResults();
          //if (i % 100 == 0) std::cout << "Round " << i << std::endl;
     }
     // ??? (TODO)
     // Gather all nodes by label
     hmap_ui_so::iterator it;
     std::set<Ocurrence>::iterator sit;
     uint nid = 0;
     for (it = voc_xp.begin(); it != voc_xp.end(); ++it) {
          nid = it->first;
          for (sit = it->second->begin(); sit != it->second->end(); ++sit) {
               Ocurrence o = *sit;
               clusters[o.getWord()].insert(nid);
          }
     }
     // Profit!  
}

void MultipleNamingGame::printResults() {
     std::set<Ocurrence>::iterator sit;
     Ocurrence o;
     hmap::iterator it;
     uint node  = 0;
     for (it = g.graph_map.begin(); it != g.graph_map.end(); ++it) {
          node = it->first;
          std::cout << g.getNodeLabel(node) << " -> ";
          for (sit = voc_xp[node]->begin(); sit != voc_xp[node]->end(); ++sit){
               o = *sit;
               std::cout << "\t" << o.getPrintable(); 
          }
          std::cout << std::endl;
     }
}

void MultipleNamingGame::printResultsByLabel() {
     // Now print them!
     std::tr1::unordered_map<std::string, std::set<uint> >::iterator cit;
     std::set<uint>::iterator siit;
     for (cit = clusters.begin(); cit != clusters.end(); ++cit) {
          std::cout << cit->first << " -> ";
          for (siit = cit->second.begin(); siit != cit->second.end();
                    ++siit) {
               std::string tmp = g.getNodeLabel(*siit);
               size_t pos;
               pos = tmp.find(" ");
               while (pos != std::string::npos) {
                    tmp.replace(pos,1,"_");
                    pos = tmp.find(" ");
               }
               std::cout << "  " << tmp;
          }
          std::cout << std::endl;
     }
}

/*
 * Calculates the Silhouette index for the clustering
 */
void MultipleNamingGame::getSilhouette() {
     std::vector<std::string> id_clusters;
     std::tr1::unordered_map<uint, std::set<uint> > val_clusters;
     // since cluster 0 is virtual, let's fake it
     id_clusters.push_back("0");
     std::tr1::unordered_map<std::string, std::set<uint> >::iterator cit;
     std::set<uint>::iterator siit;
     for (cit = clusters.begin(); cit != clusters.end(); ++cit) {
          if (cit->second.size() > 3) {
               // Keeping it for reference
               id_clusters.push_back(cit->first);
               for (siit = cit->second.begin(); siit != cit->second.end();
                         ++siit) {
                    val_clusters[id_clusters.size()-1].insert(*siit);
               }
          }
     }
     // TODO Clusters ready (I hope). What else? node_clusters

}

void MultipleNamingGame::mergeLabels(double thold, uint simfunc) {
     std::priority_queue< std::pair<uint, std::string> > c_queue;
     // Preparing to weed off the smaller clusters
     std::vector<std::string> small_clusters;
     std::tr1::unordered_map<std::string, std::set<uint> >::iterator cit;
     for (cit = clusters.begin(); cit != clusters.end(); ++cit) {
          if (cit->second.size() < 3) {
               small_clusters.push_back(cit->first);
          } else {
               c_queue.push(std::pair<uint, std::string>(0, cit->first));
          }
     }
     for (int i = 0; i < small_clusters.size(); ++i) {
          clusters.erase(small_clusters[i]);
     }
     uint priority_counter = 1;
     std::set<std::string> already_seen;
     std::string c1, c2;
     std::pair<uint, std::string> qtop;
     // Starting merge process
     while (!c_queue.empty()) {
          std::cout << "---------------------------------" << std::endl;
          std::cout << "Starting loop. Heap size: " << c_queue.size() <<
               std::endl;
          qtop = c_queue.top();
          c_queue.pop();
          c1 = qtop.second;
          std::cout << "Top Element: " << c1 << std::endl;
          // If the top cluster in the queue was not yet evaluated 
          if (already_seen.find(c1) == already_seen.end()) {
               //std::cout << "Never seen. Comparing to other clusters..\n";
               // Iterate and compare to all other clusters
               for (cit = clusters.begin(); cit != clusters.end(); ++cit) {
                    c2 = cit->first;
                    if (c1 != c2) {
                         //std::cout << "To cluster: " << c2 << std::endl;
                         double sim = doSimilarity(&clusters[c1], 
                                   &clusters[c2], simfunc);
                         //std::cout << "Sim(" << c1 << ", " << c2 << ") = " <<
                         //     sim << std::endl;
                         if (sim > thold) {
                              std::cout << "Merging! (I hope...)\n";
                              // merge c1 and c2 into a c1_c2
                              std::set<uint> c1c2; 
                              std::merge(clusters[c1].begin(), 
                                        clusters[c1].end(), 
                                        clusters[c2].begin(),
                                        clusters[c2].end(),
                                        std::inserter(c1c2, c1c2.begin()));
                              // add c1 and c2 to the already_seen set
                              already_seen.insert(c1);
                              already_seen.insert(c2);
                              // delete c1 and c2 from clusters
                              clusters.erase(c1);
                              clusters.erase(c2);
                              // Add c1_c2 to clusters
                              std::string newc(c1 + "_" + c2);
                              clusters[newc] = c1c2;
                              // Add c1_c2 to queue with a higher priority
                              ++priority_counter;
                              c_queue.push(std::pair<uint, std::string>
                                        (priority_counter, newc));
                              // Stop this clusters evaluation
                              break;
                         }
                    }
               }
               already_seen.insert(c1);
          }
     }
}


// Test for a faster merging algorithm
void MultipleNamingGame::mergeLabels2(double thold) {
     //std::priority_queue< std::pair<uint, std::string> > c_queue;
     typedef std::pair<std::string, std::set<nedge> > p_cluster_edges;
     std::vector<p_cluster_edges> c_queue;
     // Preparing to weed off the smaller clusters
     std::vector<std::string> small_clusters;
     std::tr1::unordered_map<std::string, std::set<uint> >::iterator cit;
     for (cit = clusters.begin(); cit != clusters.end(); ++cit) {
          if (cit->second.size() < 3) {
               small_clusters.push_back(cit->first);
          } else {
               // TODO pegar os edgesets aqui
               c_queue.push_back(p_cluster_edges(cit->first, 
                         getEdgeSet(&cit->second)));
          }
     }
     for (int i = 0; i < small_clusters.size(); ++i) {
          clusters.erase(small_clusters[i]);
     }
     std::set<std::string> merged;
     std::string c1, c2;
     bool DEBUG = false;
     // Starting merge process
     for (int i = 0; i < c_queue.size(); ++i) {
          std::cout << "---------------------------------" << std::endl;
          c1 = c_queue[i].first;
          std::cout << "Cluster: " << c1 << "( " << i << " / " << 
               c_queue.size() << " ) -> size = " << 
               c_queue[i].second.size() << ". Merged: " <<
              merged.size() << std::endl;
          //if (c1 == "reseda") DEBUG = true; else DEBUG = false;
          // If the top cluster in the queue was not yet evaluated 
          if (merged.find(c1) == merged.end()) {
               if (DEBUG) {
                    std::cout << "Cluster " << c1 << " never seen. \n";
               }
               // Iterate and compare to all other clusters
               for (int j = 0; j < c_queue.size(); ++j) {
                    c2 = c_queue[j].first;
                    if ((c1 != c2) && 
                              (merged.find(c2) == merged.end())) {
                         if (DEBUG) {
                              std::cout << "Comparing to " << c2 << " ...";
                         }
                         //std::cout << "To cluster: " << c2 << std::endl;
                         // TODO similarity
                         double sim = doSimilarity(&c_queue[i].second, 
                                   &c_queue[j].second);
                         //std::cout << "Sim(" << c1 << ", " << c2 << ") = " <<
                         //     sim << std::endl;
                         if (sim > thold) {
                              if (DEBUG) {
                                   std::cout << "HIT! S = " << sim << std::endl;
                              }
                              //std::cout << "Merging (" << c1 << " | "<< 
                              //    c2 << ")\n";
                              // merge c1 and c2 into a c1_c2
                              std::set<uint> c1c2; 
                              std::merge(clusters[c1].begin(), 
                                        clusters[c1].end(), 
                                        clusters[c2].begin(),
                                        clusters[c2].end(),
                                        std::inserter(c1c2, c1c2.begin()));
                              // add c2 to the already_seen set
                              // If we added c1, a possible, yet unseen, merge
                              // of c2 to a c3 would never be seen
                              merged.insert(c2);
                              // delete c1 and c2 from clusters
                              //clusters.erase(c1);
                              clusters.erase(c2);
                              // Add c1_c2 to clusters
                              //std::string newc(c1 + "_" + c2);
                              //clusters[newc] = c1c2;
                              clusters[c1] = c1c2;
                              // TODO add merged contents to c_queue->c1 
                              // for further merges
                              std::set<nedge> ec1c2;
                              std::merge(c_queue[i].second.begin(),
                                        c_queue[i].second.end(),
                                        c_queue[j].second.begin(),
                                        c_queue[j].second.end(),
                                        std::inserter(ec1c2, ec1c2.begin()));
                              c_queue[i] = p_cluster_edges(c1, ec1c2);
                              // "Baptize" new c1
                              //c1 = newc;
                         }
                    }
               }
               //already_seen.insert(c1);
          }
     }
}


double MultipleNamingGame::doSimilarity(std::set<uint>* c1, std::set<uint>* c2,
          uint func) {
     if (func == 1) {
          // Rationale here is assimilation. If the intersection between
          // the two sets is almost as big as the smallest set, than
          // it should be absorbed by the bigger one
          std::set<uint> inter;
          std::set_intersection(c1->begin(), c1->end(),
               c2->begin(), c2->end(),
               std::inserter(inter, inter.begin()));
          uint divisor = (c1->size() < c2->size())? c1->size(): c2->size();
          return inter.size()/(double)divisor;
     } else if (func == 2) {
          // Intersection namig is really only a label filter. This 
          // similarity function tries to use real (albeit basic) 
          // topological info for clustering
          // In this case, we will compare not the common nodes among both
          // clusters, but the number of common edges between them.
          std::set<nedge> edges1, edges2, inter;
          edges1 = getEdgeSet(c1);
          edges2 = getEdgeSet(c2);
          std::set_intersection(edges1.begin(), edges1.end(),
               edges2.begin(), edges2.end(),
               std::inserter(inter, inter.begin()));
          //uint divisor = edges1.size() + edges2.size() - inter.size();
          uint divisor = (edges1.size() < edges2.size())? 
               edges1.size(): edges2.size();
          return inter.size()/(double)divisor;
     } else {
          std::cout << "Unknown similarity function!\n";
          exit(1);
     }
}

double MultipleNamingGame::doSimilarity(std::set<nedge>* c1, 
          std::set<nedge>* c2) {
     std::set<nedge> inter;
     std::set_intersection(c1->begin(), c1->end(),
               c2->begin(), c2->end(),
               std::inserter(inter, inter.begin()));
     //uint divisor = edges1.size() + edges2.size() - inter.size();
     uint divisor = (c1->size() < c2->size())? 
               c1->size(): c2->size();
     return inter.size()/(double)divisor;
}


std::set<nedge> MultipleNamingGame::getEdgeSet(std::set<uint>* c) {
     std::set<uint>::iterator it;
     std::set<nedge> edges;
     for (it = c->begin(); it != c->end(); ++it) {
          std::set<Edge>* nhood = g.getAdjacency(*it);
          std::set<Edge>::iterator nit;
          for (nit = nhood->begin(); nit != nhood->end(); ++nit) {
               if (c->find(nit->getNode()) != c->end()) {
                    edges.insert(nedge(*it, nit->getNode()));
               }
          }

     }
     return edges;
}
