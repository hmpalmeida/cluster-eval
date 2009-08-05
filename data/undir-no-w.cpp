#include <iostream>
#include <fstream>
#include <cstring>

int main (int argc, char** argv) {
     std::ifstream infile;
     std::ofstream outfile;
     outfile.open("undir-no-w.txt");
     infile.open("uto-rede-geral-codBuyer-codSeller-codNegotiation-Price-Feedback-shopCategory-date.txt", std::ifstream::in);
     std::string s;
     while (!infile.eof()){
          
          infile >> s;
          if (s == "***") {
               outfile << std::endl;
          } else {
               outfile << s << "  ";
          }
     }
     infile.close();
     outfile.close();
}
