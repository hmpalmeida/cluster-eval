#include "ocurrence.hpp"
#include "utils.hpp"


Ocurrence::Ocurrence() {
     word = "";
     count = 0;
     distance = 0;
}

Ocurrence::Ocurrence(std::string w) {
     word = w;
     count = 1;
     distance = 0;
}

void Ocurrence::setWord(std::string w) {
     word = w;
}

void Ocurrence::addCounter() {
     ++count;
}

void Ocurrence::setCounter(unsigned int value) {
     count = value;
}

unsigned int Ocurrence::getCounter() {
     return count;
}

std::string Ocurrence::getWord() {
     return word;
}

bool Ocurrence::operator<(const Ocurrence &other) const {
     return (this->word < other.word);
}

bool Ocurrence::operator==(const Ocurrence &other) const {
     return (this->word == other.word);
}

Ocurrence& Ocurrence::operator=(const Ocurrence &rhs) {
     // Only do assignment if RHS is a different object from this.
     if (this != &rhs) {
          // Deallocate, allocate new space, copy values...
          this->word = rhs.word;
          this->count = rhs.count;
          this->distance = rhs.distance;
     }
     return *this;
}

Ocurrence::Ocurrence(const Ocurrence &oc) {
     word = oc.word;
     count = oc.count;
     distance = oc.distance;
}

std::string Ocurrence::getPrintable() {
     std::string s;
     s = "(" + word + ", " + to_string<uint>(count) + ", " + 
          to_string<uint>(distance) + ")";
     return s;
}

void Ocurrence::setDistance(uint value) {
     distance = value;
}

void Ocurrence::addDistance() {
     ++distance;
}

unsigned int Ocurrence::getDistance() {
     return distance;
}

bool decreasingSortOcurrence(Ocurrence i, Ocurrence j) {
     return i.getCounter() < j.getCounter();
}
