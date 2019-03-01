//
// Created by matt on 2/18/19.
//

#include <imgui.h>
#include "DifferentialGrowth.hpp"
#include "Random.hpp"


void DifferentialGrowth::update(){
  // sum forces
  vector<vec2> forces;
  for(int i=0; i < nodes.size(); i++){
    Node *cur = nodes[i];
    vector<Node*> neighbors;
    p.neighbors(cur->pos, std::back_inserter(neighbors));
    float avoidEdges = - cur->pos.x * edgeForce;


    vec2 force{0,0};
    force += vec2(avoidEdges, 0);
    force += keepNear(cur, cur->next);
    force += keepNear(cur, cur->prev);


    for(int i=0; i < neighbors.size(); i++){
      if(neighbors[i] ==  cur || neighbors[i] == cur->next || neighbors[i] == cur->prev){
        continue;
      }
      force += repel(cur, neighbors[i]);
    }
    forces.push_back(force);
  }

  // resolve forces
  p.clear();
  for(int i=0; i < nodes.size(); i++){
    nodes[i]->pos += forces[i];
    p.add(nodes[i]);
  }

  // splice in a new node
  if(Random::f() < addChance){
    int i = Random::range(0, numNodes-1);
    Node* n = new Node();
    n->pos= lerp(nodes[i]->pos, nodes[i]->next->pos, .5f);
    
    n->prev = nodes[i];
    n->next = nodes[i]->next;
    nodes[i]->next->prev = n;
    nodes[i]->next = n;
    nodes.insert(nodes.begin() + i, n);
  }
}

bool DifferentialGrowth::imSettings() {
  bool clear = false;
  STATIC_DO_ONCE(clear = true;);

  clear |= ImGui::SliderInt("numParticles", &numNodes, 0, 1000);
  clear |= ImGui::InputFloat("attractionForce", &attractForce, 0,.1);
  clear |= ImGui::InputFloat("edgeForce", &edgeForce, 0,.1);
  clear |= ImGui::InputFloat("radius", &radius, 0, .1);
  clear |= ImGui::InputFloat("desiredDist", &desiredDist, 0, .1);
  clear |= ImGui::InputFloat("repelForce", &repelForce, 0, .1);
  clear |= ImGui::InputFloat("repelDist", &repelDist, 0, .1);
  clear |= ImGui::SliderFloat("startRadius", &startRadius, 0, 1);
  clear |= ImGui::Checkbox("DrawPoints", &DrawPoints);
  clear |= ImGui::SliderInt("gammaGain", &gammaGain, 1, 100);
  ImGui::SliderFloat("AddChance", &addChance, 0, 1);

  if (clear) {
    generateNodes();
  }
  return clear;
}

void DifferentialGrowth::render(Processing* ctx){
  for(int i=0; i < nodes.size(); i++){
    float color = .5 * sin((6.28 * i)/nodes.size()) + .5  ;
    if(DrawPoints){
      ctx->ngon(nodes[i]->pos,radius, 8, {color ,color ,1,0},vec4{color ,color ,1,1} * gammaGain  );
    }
    else{
      vec3 p1 = vec3(nodes[i]->pos.x, nodes[i]->pos.y, 0);
      vec3 p2 = vec3(nodes[i]->next->pos.x, nodes[i]->next->pos.y, 0);
      ((ProcessingGL*) ctx)->line(p1,p2,vec4{color ,color ,1,1} * gammaGain, radius );
    }
  }
}

void DifferentialGrowth::generateNodes() {

  p.resize({-1,-1}, {2,2}, repelDist);
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
  float dist = glm::length(dir);
  float force = Util::rangeMap(dist, repelDist,0,0, repelForce, true);
  return -1 *  glm::normalize(dir) * force;
}
