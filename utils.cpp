#include "utils.hpp"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

// Returns a more honest random number  between 0 and hi-1
int randomNumber(int hi) { //the correct random number generator for [0,hi-1]
     // scale in range [0,1)
     const float scale = rand()/float(RAND_MAX);
     // return range [0..hi-1]
     return int(scale*hi); // implicit cast and truncation in return
}

// Returns a random alphanumeric character
char rand_alnum() {
     char c = ' ';
     uint tmp = 0;
     tmp = randomNumber(3);
     if (tmp == 0) {
          c = static_cast<char>(randomNumber(10) + 48);
     } else if (tmp == 1) {
          c = static_cast<char>(randomNumber(26) + 65);
     } else {
          c = static_cast<char>(randomNumber(26) + 97);
     }
     return c;
}

// Returns a random string of size sz
std::string rand_alnum_str (uint sz) {
     std::string s;
     char c;
     //std::cout << time(NULL) << std::endl; 
     //srand(time(NULL));
     s.reserve  (sz);
     for (int i = 0; i < sz; ++i) {
          c = rand_alnum();
          s.push_back(c);
     }
     //generate_n (std::back_inserter(s), sz, rand_alnum);
     return s;
}


void squareMatrixMultiplication(float** m, float** s, float** ms, uint n) {
     float acc = 0.0;
     for (int i = 0; i < n; ++i) {
          for (int j = 0; j < n; ++j) {
               // Let's calculate ms_{ij}
               for (int k = 0; k < n; ++k) {
                    acc += m[i][k]*s[k][j];
               }
               ms[i][j] = acc;
               acc = 0;
          }
     }
}

float sumElementsSquareMatrix(float** m, uint n){
     float acc = 0.0;
     for (int i = 0; i < n; ++i) {
          for (int j = 0; j < n; ++j) {
               acc += m[i][j];
          }
     }
     return acc;
}

void printSquareMatrix(float** m, uint n){
     for (int i = 0; i < n; ++i) {
          for (int j = 0; j < n; ++j) {
               std::cout << m[i][j] << "\t";
          }
          std::cout << std::endl;
     }
}

// Tokenizes a string. Whitespaces are the delimitors
std::vector<std::string> stringTokenizer(std::string line) {
     uint pos = 0;
     std::string t;
     std::vector<std::string> tokens;
     while (pos < line.size()) {
          t.clear();
          while (line[pos] == ' ') ++pos;
          while ((line[pos] != ' ') && (pos < line.size())) {
               t.push_back(line[pos]);
               ++pos;
          }
          if (t.size() > 0) tokens.push_back(t);
     }
     return tokens;
}

/*
template <typename T>
std::string to_string(const T& value){
     std::ostringstream oss;
     oss << value;
     return oss.str();
}
*/
