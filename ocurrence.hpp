#include <string>
//#include "utils.hpp"

class Ocurrence {
     private:
          std::string word;
          unsigned int count;
          unsigned int distance; // Distance from the label's origin
     public:
          Ocurrence();
          Ocurrence(std::string word);
          void setWord(std::string word);
          void addCounter();
          void setCounter(unsigned int value);
          unsigned int getCounter();
          void addDistance();
          void setDistance(unsigned int value);
          unsigned int getDistance();
          std::string getWord();
          bool operator<(const Ocurrence &other) const;
          bool operator==(const Ocurrence &other) const;
          Ocurrence& operator=(const Ocurrence &rhs);
          Ocurrence(const Ocurrence &oc);
          std::string getPrintable();
};

// Used to sort the <Ocurrence>s
bool decreasingSortOcurrence(Ocurrence i, Ocurrence j);
