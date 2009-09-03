#include "utils.hpp"
#include <string>

class Edge {
     protected:
          uint node;
          float weight;
     public:
          Edge();
          Edge(uint n, float w);
          ~Edge();
          void setNode(uint n);
          void setWeight(float w);
          uint getNode() const;
          float getWeight() const;
          bool operator<(const Edge& a) const;
          bool operator==(const Edge& a) const;
          std::string toString() const;
};
