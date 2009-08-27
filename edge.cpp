#include "edge.hpp"

Edge::Edge(){
}

Edge::Edge(uint n, float w){
     node = n;
     weight = w;
}

Edge::~Edge(){
}

void Edge::setNode(uint n){
     node = n;
}

void Edge::setWeight(float w){
     weight = w;
}

uint Edge::getNode(){
     return node;
}

float Edge::getWeight(){
     return weight;
}

bool Edge::operator<(const Edge& a) {
    return node < a.node;
}
