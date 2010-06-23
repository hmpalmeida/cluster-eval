#include "utils.hpp"
#include <string>
#include <set>

class Node {
     protected:
          uint id;
          std::set<std::string> attributes;
     public:
          Node();
          ~Node();
          bool setId(uint id);
          uint getId();
          bool addAttribute(std::string attr);
          bool operator==(const Node& a) const;
          bool operator<(const Node& a) const;
};
