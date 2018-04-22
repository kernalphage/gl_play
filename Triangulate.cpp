//
// Created by matt on 3/5/18.
//

#include "Triangulate.hpp"
#include "Random.hpp"
#include "imgui.h"
#include <glm/gtx/norm.hpp>

#include "ref/FastNoise/FastNoise.h"
using namespace std;
using namespace glm;

std::vector<Tri> TriBuilder::triangulate(float zoom,
                                         Processing *ctx) {
  vector<Tri> open{_curTriangles};
  vector<Tri> ret{};
  size_t cur = 0;
  Random::seed(_seed);
  FastNoise noise;
  noise.SetSeed(_seed);
  noise.SetFrequency(_frequency);
  while (open.size() > 0) {
    Tri curTri = open[0]; open.erase(open.begin());
    int side = curTri.longestSide();

    // 0 __d__ 1
    //   \ | /
    //    \|/
    //     2

    vec3 d = mix(curTri[side], curTri[side + 1],
                 Random::range(.5f - _skew, .5f + _skew));

    // ctx->line(d, curTri[side+2]);
    float newDepth = curTri.depth + Random::range(_decayMin, _decayMax);
    if (newDepth < _maxDepth + _zoom/2.0f) {
      open.push_back(Tri{curTri[side], curTri[side + 2], d, newDepth});
      open.push_back(Tri{curTri[side + 2], d, curTri[side + 1], newDepth});
    } else {
      ret.push_back(curTri);
    }
    cur++;
  }

  vector<Tri> to_draw;
  std::copy_if(ret.begin(), ret.end(), std::back_inserter(to_draw), 
    [=](Tri t){ return length(t.center() * _zoom) < _radius; });

  auto sampColor = [=](vec3 pos) {
  return UI_Vertex{
    pos, mix(_minColor, _maxColor, noise.GetNoise(pos.x, pos.y))};
  };

  for(auto t: to_draw){
      ctx->tri(sampColor(t.a * _zoom), sampColor(t.b * _zoom), sampColor(t.c* _zoom));
  }

  _curTriangles = to_draw;
}

bool TriBuilder::imSettings() {
  bool redraw = false;
  float prevZoom = _zoom;
  redraw |= ImGui::SliderFloat("Zoom", &_zoom, 0, 10);

  if(ImGui::Button("Reset") || prevZoom < _zoom){
      _curTriangles = {Tri{{0, .8, 0}, {-.8, -.8, 0}, {.8, -.8, 0}, 0}};
      redraw = true;
  }

  ImGui::Text("Num Triangles = %d", (int)_curTriangles.size());
  redraw |= ImGui::InputInt("Seed", &_seed);
  redraw |= ImGui::SliderFloat("Max Depth", &_maxDepth, 0.0f, 12.0f);
  redraw |= ImGui::DragFloatRange2("Decay", &_decayMin, &_decayMax, .01f, .01f,
                                   _maxDepth);
  redraw |= ImGui::SliderFloat("Skew", &_skew, 0.f, .5f);
  redraw |= ImGui::SliderFloat("Radius", &_radius, 0.f, 2.f);
  redraw |= ImGui::SliderFloat("Frequency", &_frequency, 0.f, 100.f);
  redraw |= ImGui::ColorEdit4("MinColor", (float *)&_minColor);
  redraw |= ImGui::ColorEdit4("MaxColor", (float *)&_maxColor);
  return redraw;
}

// Longest side on the triangle
int Tri::longestSide() {
  vector<float> d = {distance2(a, b), distance2(b, c), distance2(c, a)};
  auto maxelem = max_element(d.begin(), d.end());
  return std::distance(d.begin(), maxelem); // random() % 3;
}



bool testTriangle() {

  Tri tri{};
  tri.c = vec3{2, 3, 0};
  tri[2].y = 4;
  assert(tri.c.y == 4);

  return false;
}
