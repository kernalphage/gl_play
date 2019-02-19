//
// Created by matt on 2/18/19.
//

#include <imgui.h>
#include "DifferentialGrowth.hpp"

void DifferentialGrowth::update(){
  // sum forces
  vector<vec2> forces;
  for(int i=0; i < nodes.size(); i++){
    Node *cur = nodes[i];
    vector<Node*> neighbors;
    //p.neighbors(cur->pos, std::back_inserter(neighbors));
    vec2 force{0,0};
    force += keepNear(cur, cur->next);
    force += keepNear(cur, cur->prev);

    for(int i=0; i < neighbors.size(); i++){
      if(neighbors[i] ==  cur || neighbors[i] == cur->next || neighbors[i] == cur->prev){
        continue;
      }
      //force += repel(cur, neighbors[i]);
    }
    forces.push_back(force);
  }

  // resolve forces
 // p.clear();
  for(int i=0; i < nodes.size(); i++){
    nodes[i]->pos += forces[i];
    //p.add(nodes[i]);
  }
}

bool DifferentialGrowth::imSettings() {
  bool clear = false;
  STATIC_DO_ONCE(clear = true;);

  clear |= ImGui::SliderInt("numParticles", &numNodes, 0, 1000);
  clear |= ImGui::InputFloat("attractionForce", &attractForce, 0,.1);
  clear |= ImGui::InputFloat("radius", &radius, 0, .1);
  clear |= ImGui::InputFloat("desiredDist", &desiredDist, 0, .1);
  clear |= ImGui::SliderFloat("startRadius", &startRadius, 0, 1);

  if (clear) {
    generateNodes();
  }
  return clear;
}

void DifferentialGrowth::render(Processing* ctx){
  for(int i=0; i < nodes.size(); i++){
    ctx->ngon(nodes[i]->pos,radius, 8, {1,1,1,0},{1,1,1,1}  );
  }
}

void DifferentialGrowth::generateNodes() {
  p.clear();
  clearNodes();
  float di = 6.28f / numNodes;
  for (int i = 0; i < numNodes; i++) {
    Node *n = new Node();
    if (i != 0) {
      n->prev = nodes[i - 1];
      nodes[i - 1]->next = n;
    }
    n->pos = vec2(sin(i * di), cos(i * di)) *  startRadius;
    nodes.push_back(n);
  }
  nodes[0]->prev = nodes[numNodes -1];
  nodes[numNodes-1]->next = nodes[0];

}

vec2 DifferentialGrowth::keepNear(const DifferentialGrowth::Node *a, const DifferentialGrowth::Node *b) {
  const vec2 dir = b->pos - a->pos;
  float dist = glm::length(dir);
  return glm::normalize(dir) * (dist - desiredDist) * attractForce;
}

vec2 DifferentialGrowth::repel(const DifferentialGrowth::Node *a, const DifferentialGrowth::Node *b) {
  const vec2 dir = b->pos - a->pos;
  return -1 *  glm::normalize(dir) * repelForce;
}
