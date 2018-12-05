//
// Created by matt on 8/15/18.
//

#include "Streamline.hpp"
#include "Processing.hpp"
#include <iostream>
#include "imgui.h"
using namespace std;

void Streamline::render(Processing *ctx) {
  for(const auto line: lines){
    vector<vec3> spline;
    auto cur = line;
    while(cur != nullptr){

      spline.push_back({SPLAT2(cur->pos), 0});
      cur = cur->next;
    }
    ctx->spline(spline,{1,1,1,1}, {1,0,0,1}, lineWidth);
  }
}

bool Streamline::isvalid(vec2 pt)
{

    if(pt.x < 0 || pt.y < 0){
      return false;
    }
    if(pt.x > width || pt.y > height){
      return false;
    }
  return true;
  /*
    vector<Node *> neigh;
    p.neighbors(pt, std::back_inserter(neigh));


    const auto distfn = [=]( Node* next ){ return glm::distance2(next->pos, pt);};
    float nodeDist = std::accumulate(neigh.begin(), neigh.end(), distfn(neigh[0]), [=](float d, Node* n){
      return std::min(d, distfn(n));
    });

    return nodeDist > minDist;
*/
  }

Streamline::Node *Streamline::stream_point(vec2 startPos) {


    Node* start = new Node{nullptr, startPos, false};
    Node* cur = start;
    bool backwards = false;
    int cutoff = 120;
    while( cutoff > 0 ){
      cutoff--;
      vec2 nextp = next(cur, backwards);
      if(!isvalid(nextp)) {
        if(!backwards){
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
      }else{
        cur->next = nextNode;
      }
      cur = nextNode;
    }

    lines.push_back(cur);
    return cur;
  }

vec2 Streamline::next(Streamline::Node *pos, bool backwards) {

  vec2 dir = vec2{.2f,.3f} - pos->pos;
  dir = vec2{-dir.y, dir.x};
  dir.x += pos->pos.x * windStrength;
  if (backwards) {
    return pos->pos + normalize(dir) * stepDist;
  } else {
    return pos->pos - normalize(dir) * stepDist;
  };
}

bool Streamline::imSettings() {

  bool redraw = false;

  redraw |= ImGui::SliderFloat("lineWidth", &lineWidth, 0.001, 0.01);
  redraw |= ImGui::SliderFloat("stepDist", &stepDist, 0.001, 0.01);
  redraw |= ImGui::SliderFloat("windstrength", &windStrength, .01, 1);
  return redraw;
}

void Streamline::destroy() {
  //hoo boy
  for(Node* l:lines){
    Node* cur = l;
    while(cur != nullptr){
      Node* next = cur->next;
      delete(cur);
      cur = next;
    }
  }
  lines.clear();
}
