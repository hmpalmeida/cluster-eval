#ifndef CLUSTERING_SCAN_INTERSECTION_NAMING_H_
#define CLUSTERING_SCAN_INTERSECTION_NAMING_H_

#include "multiple_naming.hpp"
#include <string>
#include <tr1/unordered_map>

typedef std::tr1::unordered_map<uint, bool> hmap_ui_b;

/* One more extension for the naming game. Now, instead of doing random
 * choices, we will iterate through all nodes in one round. More than that,
 * communication will result in both nodes keeping the intersection of
 * their vocabulary on successful cases. In unsuccesssful cases we do not
 * know yet what to do, but will try different approaches...
 */

class IntersectionNamingGame : public MultipleNamingGame {
     protected:
          // Communicate has a different syntax from the other NGs
          bool communicate(uint sender, uint receiver);
          hmap_ui_so tmp_voc_xp;
          hmap_ui_b comm_success;
          void swapVocabulariesXP();
          void addTmpVocabularyXP(uint node, Ocurrence o, bool move = false);
          Ocurrence getBestWord(uint node); 
     public:
          IntersectionNamingGame();
          IntersectionNamingGame(std::string filename);
          ~IntersectionNamingGame();
          void run(uint rounds);
};

#endif  // CLUSTERING_SCAN_INTERSECTION_NAMING_H_
