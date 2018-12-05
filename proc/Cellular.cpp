//
// Created by matt on 11/25/18.
//

#include "Cellular.hpp"
#include <algorithm>
#include <imgui.h>
#include "../Random.hpp"
#include <iostream>
using namespace std;

void Cellular::genPts() {
  pts.clear();
  edges.clear();
  Random::seed(m_seed);

  for(int y=0; y < m_numpts; y++){
  for(int x =0; x < m_numpts; x++){
      CellNode newcell{};
      newcell.pos = vec3{x,y,0} * 2.0f/m_numpts - vec3{.8,.8,0};
      newcell.outside = (y == 0) || (x == 0) || (x == m_numpts-1) || (y == m_numpts - 1);
      newcell.color = vec4{0,.41f,.3f,100};

      size_t curIdx = pts.size();
      pts.push_back(newcell);
      if(x > 0){
        edges.emplace_back(curIdx, y * m_numpts + x-1);
      }
      if(y > 0){
        edges.emplace_back(curIdx, (y-1)* m_numpts + x);
      }
    };
  }

  // randomly delete some nodes
  edges.erase( std::remove_if(edges.begin(), edges.end(),
        [=](auto edge){return Random::f() > m_removeChance;}), edges.end());
  vec3 pos = Random::random_point({-1,-1,0}, {1,1,0});
  vec3 dir = Random::random_point({-1,-1,0}, {1,1,0});
  dir = normalize(dir);
  dir *= 3;
  add_edge(pos, dir);

}

void Cellular::render(Processing *ctx, bool &redraw, bool &clear, int curFrame, int maxFrames) {
  ctx->clear();
  STATIC_DO_ONCE(redraw = true);
  imSettings(redraw, clear);
  if(redraw){
    genPts();
  }

  if(Random::f() < m_edgeChance){
    vec3 pos = Random::random_point({-1,-1,0}, {1,1,0});
    vec3 dir = Random::random_point({-1,-1,0}, {1,1,0});
    dir = normalize(dir);
    dir *= 3;
    add_edge(pos, dir);
  }
  settle();
  clear = true;
  redraw = true;
  // todo: add warmup
  for(std::pair<int, int> edge : edges){
    CellNode a = pts[edge.first];
    CellNode b = pts[edge.second];
    ((ProcessingGL*) ctx)->line(a.pos, b.pos, a.color, .001);
  }
  ctx->flush();
}

Cellular::Cellular() {
  genPts();
}

void Cellular::imSettings(bool& redraw, bool& clear) {
  redraw |= ImGui::SliderFloat("drop percent", &m_removeChance, 0, 1);
  redraw |= ImGui::SliderFloat("edge add chance", &m_edgeChance, 0, 1);
  redraw |= ImGui::SliderInt("numpts", &m_numpts, 2, 100);
  redraw |= ImGui::SliderFloat("settleAmount", &m_settleAmount, 0.0001, 0.1);
  redraw |= ImGui::InputInt("seed", &m_seed);
  clear = redraw;
}

void Cellular::settle() {

  vector<CellNode> posPrime(pts);

  for(std::pair<int, int> edge : edges){
    vec3 a = pts[edge.first].pos;
    vec3 b = pts[edge.second].pos;
    CellNode& ap = posPrime[edge.first];
    CellNode& bp = posPrime[edge.second];

    vec3 fa = lerp(a,b, m_settleAmount) - a;
    vec3 fb = lerp(a,b, 1 - m_settleAmount) - b;
    if(!ap.outside)
      ap.pos += fa;
    if(!bp.outside)
      bp.pos += fb;
  }
  pts = posPrime;
}

void Cellular::add_edge(const vec3 pos, const vec3 dir) {

  std::pair<float, int> edge1{1.0, -1};
  std::pair<float, int> edge2{1.0, -1};
  for(int i=0; i < edges.size(); i++){
    auto e = edges[i];
    float n1 = Geo::line_intersect( pos, pos+dir, pts[e.first].pos, pts[e.second].pos);
    if((n1 > 0)  && n1 <= edge1.first){
      edge1.first = n1;
      edge1.second = i;
    }
    float n2 = Geo::line_intersect(pos, pos-dir, pts[e.first].pos, pts[e.second].pos);
    if((n2 > 0) && n2 <= edge2.first){
      edge2.first = n2;
      edge2.second = i;
    }
  };

  // set color, blah blah
  CellNode n1;
  n1.pos = pos + edge1.first * (dir);
  size_t i1 = pts.size();
  pts.push_back(n1);

  CellNode n2;
  n2.pos = pos - edge2.first * (dir);
  size_t i2 = pts.size();
  pts.push_back(n2);

// pair up indices
  if(edge1.second > 0) {
    int temp = edges[edge1.second].first;
    edges[edge1.second].first = i1;
    edges.emplace_back(i1, temp);
  }
// pair up indices
  if(edge2.second > 0) {
    int temp = edges[edge2.second].first;
    edges[edge2.second].first = i2;
    edges.emplace_back(i2, temp);
  }
 edges.emplace_back(i1, i2);
}
