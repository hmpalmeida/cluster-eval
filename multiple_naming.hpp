#ifndef CLUSTERING_SCAN_MULTIPLE_NAMING_H_
#define CLUSTERING_SCAN_MULTIPLE_NAMING_H_

#include "naming.hpp"
#include "ocurrence.hpp"
#include <string>
#include <set>
#include <tr1/unordered_map>

typedef std::tr1::unordered_map<uint, std::set<Ocurrence>* > hmap_ui_so;

/*
 * Extended naming game. Will not drop all vocabulary on successful
 * communication.
 */
class MultipleNamingGame : public NamingGame {
     protected:
          uint max_voc_size;
          uint num_drops;    // Number of dropped elements on OK communication
          hmap_ui_so voc_xp; // Expanded vocabulary
          bool communicate(uint sender, uint receiver, std::string word);
          std::string chooseWord(uint node);
          std::set<Ocurrence>* getVocabularyXP(uint node);
          Ocurrence popWordXP(uint node, std::string word);
          void dropVocabularyXP(uint node, uint num);
          void addOcurrenceXP(uint node, Ocurrence o);
          double doSimilarity(std::set<uint>* c1, std::set<uint>* c2, 
                    uint func);
     public:
          MultipleNamingGame();
          MultipleNamingGame(std::string filename, 
                    uint max_v_size = 9999, uint drops = 1);
          ~MultipleNamingGame();
          void run(uint rounds);
          void printResults();
          void printResultsByLabel();
          void getSilhouette();
          void mergeLabels(double thold = 0.5, uint simfunc = 1); 
};

#endif  // CLUSTERING_SCAN_MULTIPLE_NAMING_H_
