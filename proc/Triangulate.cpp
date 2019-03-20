//
// Created by matt on 3/5/18.
//

#include "Triangulate.hpp"
#include "Random.hpp"
#include "imgui.h"
#include <glm/gtx/norm.hpp>
#include <fmt/format.h>
#include <iostream>
#include "ref/FastNoise/FastNoise.h"
using namespace std;

std::vector<Tri> TriBuilder::triangulate(Processing *ctx, int framenum) {
    vector<Tri> open{_curTriangles};
  vector<Tri> ret{};
  size_t cur = 0;
  Random::seed(_seed);
  FastNoise noise;
  noise.SetSeed(_seed);
  noise.SetFrequency(_frequency);
  while (open.size() > 0 && cur < 40000) {
    Tri curTri = open[0]; open.erase(open.begin());
    int side = curTri.longestSide();

    // 0 __d__ 1
    //   \ | /
    //    \|/
    //     2

    vec3 d = mix(curTri[side], curTri[side + 1],
                 Random::range(.5f - _skew, .5f + _skew));

    auto resizeTri = [&](const Tri& t){
      float skew = Random::range(_resizeRange.x, _resizeRange.y);
      auto c = t.center();
      auto da = t.a - c;
      auto db = t.b - c;
      auto dc = t.c - c;

      return Tri{skew * da + c, skew * db + c, skew * dc + c, t.depth}; 
    };

    // ctx->line(d, curTri[side+2]);
    float newDepth = curTri.depth + Random::range(_decayMin, _decayMax);
    if (newDepth <= _maxDepth ) {
      Tri left = Tri{curTri[side], curTri[side + 2], d, newDepth};
      Tri right = Tri{curTri[side + 2], d, curTri[side + 1], newDepth};
      left = resizeTri(left);
      right = resizeTri(right);
      open.push_back(left);
      open.push_back(right);

      if(_drawLeftParent)
        ret.push_back(left);
      if(_drawRightParent)
        ret.push_back(right);
    } else {
      ret.push_back(curTri);
    }
    cur++;
  }
  int idx = _seed;
  vector<Tri> to_draw;
  auto shouldDraw =
      [=](Tri t){
        auto ppos = t.center() ;
        ppos.y *=-1;
        ppos = (ppos * .5f + vec3(.5f, .5f, 0) );
        auto dist = Util::median(Util::sampleDistField({ppos.x, ppos.y}, idx));
        return  dist/255.0f > _radius; };

  std::copy_if(ret.begin(), ret.end(), std::back_inserter(to_draw), shouldDraw);
  std::copy_if(open.begin(), open.end(), std::back_inserter(to_draw), shouldDraw);

  auto sampColor = [&](vec3 pos) {
    auto ppos = (pos * .5 + vec3(.5f, .5f, 0) ) * _radius;
    auto dist = Util::median(Util::sampleDistField({ppos.x, ppos.y}, idx));
  auto m = mix(_minColor, _maxColor, .75f);
    m *= _colorGain;
    return m;
  };

  auto triVert = [&](const vec3 pos){
    return UI_Vertex{pos, sampColor(pos)};
  };

  for(auto t: to_draw){
    if(Random::f() < _fillChance){
     ctx->tri(triVert(t.a), triVert(t.b), triVert(t.c));
    }
    else{
      ((ProcessingGL*) ctx)->line(t.a, t.b, sampColor(t.a),_linethickness);
      ((ProcessingGL*) ctx)->line(t.b, t.c, sampColor(t.b),_linethickness);
      ((ProcessingGL*) ctx)->line(t.c, t.a, sampColor(t.c),_linethickness);      
   }
  }
  return _curTriangles;
}

bool TriBuilder::imSettings() {

  bool redraw = false;

  ImGui::Text("Num Triangles = %d", (int)_curTriangles.size());
  redraw |= ImGui::InputInt("Seed", &_seed);
  if(ImGui::Button("prevJump")) {
    redraw = true;
    _seed -= DEPTH_MAP_COUNT;
  }
  ImGui::SameLine();
  if(ImGui::Button("nextJump")) {
    redraw = true;
    _seed += DEPTH_MAP_COUNT;
  }

  redraw |= ImGui::SliderFloat("Max Depth", &_maxDepth, 0.0f, 25.0f);
  redraw |= ImGui::DragFloatRange2("Decay", &_decayMin, &_decayMax, .4f, .4f,
                                   _maxDepth);
  redraw |= ImGui::SliderFloat("Skew", &_skew, 0.f, .5f);
  redraw |= ImGui::SliderFloat2("ResizeRange", (float*) &_resizeRange, 0.f, 2.0f);
  redraw |= ImGui::SliderFloat("Radius", &_radius, 0.f, 2.f);
  redraw |= ImGui::SliderFloat("FillChance", &_fillChance, 0.0f, 1.0f);
  redraw |= ImGui::ColorEdit4("MinColor", (float *)&_minColor);
  redraw |= ImGui::ColorEdit4("MaxColor", (float *)&_maxColor);
  redraw |= ImGui::SliderFloat("ColorGain", &_colorGain, 0, 100);
  redraw |= ImGui::InputFloat("thickness", &_linethickness, 0, .1);
  redraw |= ImGui::Checkbox("drawLeftParent", &_drawLeftParent);
  redraw |= ImGui::Checkbox("drawRightParent", &_drawRightParent);
  if(redraw){

          _curTriangles = {Tri{{.8, .8, 0}, {-.8, .8, 0}, {.8, -.8, 0}, 0},
                           Tri{{-.8, -.8, 0}, {-.8, .8, 0}, {.8, -.8, 0}, 0}};
  }
  // -1 1    1 1
  // -1 -1   1 -1

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
