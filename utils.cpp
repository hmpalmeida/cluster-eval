#include "utils.hpp"
#include <string.h>

void squareMatrixMultiplication(float** m, float** ms, uint n) {
     float acc = 0.0;
     for (int i = 0; i < n; ++i) {
          for (int j = 0; j < n; ++j) {
               // Let's calculate ms_{ij}
               for (int k = 0; k < n; ++k) {
                    acc += m[i][k]*m[k][j];
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
