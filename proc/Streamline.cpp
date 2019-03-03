//
// Created by matt on 8/15/18.
//

#include "Streamline.hpp"
#include "Processing.hpp"
#include <iostream>
#include <Random.hpp>
#include "imgui.h"
#include <FastNoise/FastNoise.h>
using namespace std;

void Streamline::render(Processing *ctx) {
  for(const auto line: lines) {
    vector<vec3> spline;
    auto cur = line;
    while (cur != nullptr) {
      spline.push_back({SPLAT2(cur->pos), 0});
      cur = cur->next;
    }
    ctx->spline(spline, {100, 10, 1, 1000}, {10, 10, 10, 100}, lineWidth);
  }
  lines.clear();
}

bool Streamline::isvalid(vec2 pt, Node* prev )
{
    if(pt.x < -1 || pt.y < -1){
      return false;
    }
    if(pt.x > width || pt.y > height){
      return false;
    }

    vector<Node *> neigh;
    p.neighbors(pt, std::back_inserter(neigh));

    if(neigh.size() == 0){ return true;}
    const auto distfn = [=]( Node* next ){ return glm::distance2(next->pos, pt);};
    float nodeDist = std::accumulate(neigh.begin(), neigh.end(), distfn(neigh[0]), [=](float d, Node* n){
      if(n == prev) return d;
      return std::min(d, distfn(n));
    });

    return nodeDist >= (distFactor * stepDist);
}

vec2 Streamline::nextOpenPoint() {

  auto isOpen =  [](Node* n){ return n->open; };
  std::partition(p._all.begin(), p._all.end(),isOpen);

  for(int i=0; i < p._all.size(); i++){
    Node* n = p._all[i];
    if(n->open == false){
      break;
    }
    auto dir = next(n,false);

    dir = vec2{dir.y, dir.x};
    if(isvalid(n->pos + dir)){
      return n->pos+dir;
    }
    if(isvalid(n->pos - dir)){
      return n->pos-dir;
    }
    n->open = false;
  }

  // TODO: optional for exit strategy
  return vec2{-1,-1};
}

Streamline::Node *Streamline::stream_point(vec2 startPos) {
    if(!isvalid(startPos)){ return nullptr; };

    Node* start = new Node{nullptr, startPos, true};
    p.add(start);
    Node* cur = start;
    bool backwards = false;
    int cutoff = maxIterations;
    while( true ){
      cutoff--;
      vec2 nextp = next(cur, backwards);
      if(!isvalid(nextp, cur) || cutoff <= 0) {
        if(!backwards){
          cutoff = maxIterations;
          backwards = true;
          cur = start;
          continue;
        }
        else{
          break;
        }
      }

      Node* nextNode = new Node{nullptr, nextp, false};
      p.add(nextNode);
      if(backwards){
        nextNode->next = cur;
      } else {
        cur->next = nextNode;
      }
      cur = nextNode;
    }
  // keep track of average length based on cur;
    lines.push_back(cur);
   return cur;
  }

vec2 Streamline::next(Streamline::Node *pos, bool backwards) {

  vec2 noiseP = pos->pos * windStrength;
  float x = f.GetNoise(noiseP.x, noiseP.y, 0);
  float y = f.GetNoise(noiseP.x, noiseP.y, 1354);
  vec2 dir{x,y};

  if (backwards) {
    return pos->pos + normalize(dir) * stepDist;
  } else {
    return pos->pos - normalize(dir) * stepDist;
  };
}

bool Streamline::imSettings() {

  bool redraw = false;
  redraw |= ImGui::InputInt("Seed", &seed, 0, 1);
  redraw |= ImGui::SliderFloat("lineWidth", &lineWidth, 0.001, 0.01);
  redraw |= ImGui::SliderFloat("stepDist", &stepDist, 0.001, 0.1);
  redraw |= ImGui::InputFloat("distFactor", &distFactor, 0, 1);
  redraw |= ImGui::SliderFloat("windstrength", &windStrength, .01, 110);
  redraw |= ImGui::SliderInt("Max Iterations", &maxIterations, 10, 1000);

  if(ImGui::Button("Add line")){
      vec2 pt = nextOpenPoint();
      stream_point(pt);
  }
  if(redraw){
    destroy();
  }
  return redraw;
}

void Streamline::destroy() {
  for(Node* l: p._all){
      delete(l);
  }
  lines.clear();
  backlines.clear();
  p.resize({-1,-1}, {2,2}, stepDist * distFactor);
  f.SetSeed(seed);
}
