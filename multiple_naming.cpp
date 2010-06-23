#include "multiple_naming.hpp"
#include <vector>
#include <algorithm>

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

// TODO Arrumar para que a variavel num_drops exista. Poderia adaptar tudo
// para usar a mesma função para liberar espaço quando o vocabulario chegar
// no limite?

std::set<Ocurrence>* MultipleNamingGame::getVocabularyXP(uint node) {
     return voc_xp[node];
}

Ocurrence MultipleNamingGame::popWordXP(uint node, std::string word) {
     std::set<Ocurrence>::iterator it;
     Ocurrence o(word);
     it = voc_xp[node]->find(o);
     if (it == voc_xp[node]->end()) {
          //std::cout << "Didn\'t find word: " << o.getWord() << std::endl;
          //std::cout << "Among " << voc_xp[node]->size() <<  " elements: ";
          //std::set<Ocurrence>::iterator it2;
          //Ocurrence xx;
          //for (it2 = voc_xp[node]->begin(); it2 != voc_xp[node]->end(); ++it2){
          //     xx = *it2;
          //     std::cout << " , " << xx.getPrintable(); 
          //}
          //std::cout << std::endl;
          // <node> does not know about the bird
          Ocurrence empty_o;
          return empty_o;
     } else {
          //std::cout << "Found word: " << o.getWord() << std::endl;
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
     // FIXME TESTING
     /*     
     for (int i = 0; i < vec.size(); ++i) {
               std::cout << vec[i].getPrintable() << std::endl;
     }
     std::cout << "Will remove: " << vec[0].getPrintable() << std::endl;
     */
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
     std::tr1::unordered_map<std::string, std::set<uint> > clusters;
     hmap_ui_so::iterator it;
     std::set<Ocurrence>::iterator sit;
     uint nid = 0;
     // First gather all nodes by label
     for (it = voc_xp.begin(); it != voc_xp.end(); ++it) {
          nid = it->first;
          for (sit = it->second->begin(); sit != it->second->end(); ++sit) {
               Ocurrence o = *sit;
               clusters[o.getWord()].insert(nid);
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

