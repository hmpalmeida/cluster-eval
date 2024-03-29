#include "node.hpp"

/********************************************************************
  * Empty Constructor
  ********************************************************************/
Node::Node() {
     
}

/********************************************************************
  * Destructor
  ********************************************************************/
Node::~Node() {
}


/********************************************************************
  * Sets de node Id valuer
  ********************************************************************/
bool Node::setId(uint nid) {
     id = nid;
     return 1;
}

/********************************************************************
  * Returns the node Id
  ********************************************************************/
uint Node::getId() {
     return id;
}

/********************************************************************
  * Adds a new attribute
  ********************************************************************/
bool Node::addAttribute(std::string attr) {
     std::pair< std::set<std::string>::iterator, bool > ret;
     ret = attributes.insert(attr);
     return ret.second;
}

/********************************************************************
  * Overload of the "==" operator
  ********************************************************************/
bool Node::operator==(const Node& a) const {
          return id == a.id;
}

/********************************************************************
  * Overlod of the "<" operator
  ********************************************************************/
bool Node::operator<(const Node& a) const {
         return id < a.id;
}
