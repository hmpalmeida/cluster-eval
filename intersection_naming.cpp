#include "intersection_naming.hpp"
#include <algorithm>

/*
 * Basic constructor
 */
IntersectionNamingGame::IntersectionNamingGame()
     : MultipleNamingGame() {
     // Create the tmp_voc_xp
     hmap::iterator it;
     uint node = 0;
     for (it = g.graph_map.begin(); it != g.graph_map.end(); ++it) {
          node = it->first;
          tmp_voc_xp[node] = new std::set<Ocurrence>;
          comm_success[node] = false;
     }    
}

/*
 * Constructor that already loads the graph. Equal to the parent constructor
 */
IntersectionNamingGame::IntersectionNamingGame(std::string filename)
     : MultipleNamingGame(filename) {
     // Create the tmp_voc_xp     
     hmap::iterator it;
     uint node = 0;
     for (it = g.graph_map.begin(); it != g.graph_map.end(); ++it) {
          node = it->first;
          tmp_voc_xp[node] = new std::set<Ocurrence>;
          comm_success[node] = false;
     }    
}

/*
 * Basic destructor
 */
IntersectionNamingGame::~IntersectionNamingGame(){
     // Delete the tmp_voc_xp 
     hmap::iterator it;
     uint node = 0;
     for (it = g.graph_map.begin(); it != g.graph_map.end(); ++it) {
          node = it->first;
          delete tmp_voc_xp[node];
     }    
}


/*
 * Handles the communication between two nodes. Will do an intersection
 * of both nodes' vocabularies
 */
bool IntersectionNamingGame::communicate(uint sender, uint receiver){
     std::set<Ocurrence> intersect;
     std::set<Ocurrence>* sender_voc = getVocabularyXP(sender);
     std::set<Ocurrence>* receiver_voc = getVocabularyXP(receiver);
     std::set_intersection(sender_voc->begin(), sender_voc->end(),
               receiver_voc->begin(), receiver_voc->end(),
               std::inserter(intersect, intersect.begin()));
     if (intersect.size() > 0) {
          // Success! We will add all common words to the next_round's voc
          // Remember: we do not want to lose track of the "seen" counter
          // FIXME Not right yet. Will not drop anything until the end.
          // For now we will only add things to an temporary vocabulary
          std::set<Ocurrence>::iterator it;
          for (it = intersect.begin(); it != intersect.end(); ++it) {
               Ocurrence o = *it;
               addTmpVocabularyXP(sender, o);
               //addTmpVocabularyXP(receiver, o);
               comm_success[sender] = true;
          }
          return true;
     } else {
          // Failure :( What should we do?
          // Adding the "best" label
          /*
          Ocurrence o;
          o = getBestWord(receiver);
          //o = getBestWord(sender);
          // resetting it's counter
          o.setCounter(1);
          addTmpVocabularyXP(sender, o, true);
          //addTmpVocabularyXP(receiver, o, true);
          */
          return false;
     }
}


/*
 * The main naming game loop
 */
void IntersectionNamingGame::run(uint rounds){
     hmap::iterator graph_it;
     srand(time(NULL));
     std::set<Edge>* adj;
     std::set<Edge>::iterator edges_it;
     uint sender = 0, receiver = 0;
     for (int i = 0; i < rounds; ++i) {
          /*
          // run "traditional" NG with a twist! 
          // Choose one random node (sender)
          sender = chooseNode();
          // Choose one random neighbor (receiver)
          adj = g.getAdjacency(sender);
          for (edges_it = adj->begin(); edges_it != adj->end(); ++edges_it) {
               receiver = edges_it->getNode();
               communicate(sender, receiver);
          }
          */
          /*
          receiver = chooseNeighbor(sender);
          if (receiver == -1) {
               --i;
               continue;
          }
          communicate(sender,receiver);
          */          
          
          // Run through all the nodes
          for (graph_it = g.graph_map.begin(); 
                    graph_it != g.graph_map.end(); ++graph_it) {
               sender = graph_it->first;
               for (edges_it = graph_it->second->begin();
                         edges_it != graph_it->second->end(); ++edges_it) {
                    receiver = edges_it->getNode();
                    communicate(sender, receiver);
               }
          }
          
          swapVocabulariesXP();
     }
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
}


/*
 * Add <Ocurrence> to the <tmp_voc_xp> 
 */
void IntersectionNamingGame::addTmpVocabularyXP(uint node, Ocurrence o, 
          bool move){
     std::set<Ocurrence>::iterator it;
     it = tmp_voc_xp[node]->find(o);
     if (it == tmp_voc_xp[node]->end()) {
          // New word. Add it
          if (move) o.addDistance();
          tmp_voc_xp[node]->insert(o);
     } else {
          // Already seen word. Add to the counter accordingly and then add it
          Ocurrence old = *it;
          uint count = (o.getCounter() < old.getCounter())? old.getCounter()
               : o.getCounter();
          ++count;
          o.setCounter(count);
          tmp_voc_xp[node]->erase(it);
          tmp_voc_xp[node]->insert(o);
     }
}

Ocurrence IntersectionNamingGame::getBestWord(uint node) {
     std::vector<Ocurrence> vec(voc_xp[node]->begin(), voc_xp[node]->end());
     std::sort(vec.begin(), vec.end(), decreasingSortOcurrence);
     return vec[vec.size()-1];
}

/*
 * Swaps <voc_xp> with <tmp_voc_xp>, clearing the second for the next round 
 */
void IntersectionNamingGame::swapVocabulariesXP(){
     uint node = 0;
     hmap_ui_so::iterator it;
     for (it = voc_xp.begin(); it != voc_xp.end(); ++it) {
          node  = it->first;
          // TODO Since we don't want to keep singletons, why not clearing
          // the node's vocabuary in the case of communication insuccess?
          //if (comm_success[node]) {
               // Success! Swap vocabularies
               delete voc_xp[node];
               voc_xp[node] = tmp_voc_xp[node];
               tmp_voc_xp[node] = new std::set<Ocurrence>;
          /*
          } else {
               // Communication failed. Add new words to <voc_xp>
               voc_xp[node]->insert(tmp_voc_xp[node]->begin(), 
                         tmp_voc_xp[node]->end());
               tmp_voc_xp[node]->clear();
          }
          // Reset the communication success flag
          comm_success[node] = false;
          */
     }
}

