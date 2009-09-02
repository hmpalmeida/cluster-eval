#include "edge.hpp"
#include <iostream>

Edge::Edge() {
}

Edge::Edge(uint n, float w) {
     node = n;
     weight = w;
}

Edge::~Edge() {
}

void Edge::setNode(uint n) {
     node = n;
}

void Edge::setWeight(float w) {
     weight = w;
}

uint Edge::getNode() {
     return node;
}

float Edge::getWeight() {
     return weight;
}

bool Edge::operator<(const Edge& a) const {
    return node < a.node;
}

std::string Edge::toString() const {
     std::string s("(");
     s += to_string(node);
     s += ",";
     s += to_string(weight);
     s += ")";
     return s;
}

bool Edge::operator==(const Edge& a) const {
     return node == a.node;
}
