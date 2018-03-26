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

std::vector<Tri> TriBuilder::triangulate(const std::vector<Tri> &seed,
                                         Processing *ctx) {
  vector<Tri> ret{seed};
  size_t cur = 0;
  Random::seed(_seed);
  FastNoise noise;
  noise.SetSeed(_seed);
  noise.SetFrequency(_frequency);
  while (cur < ret.size()) {
    Tri curTri = ret[cur];
    int side = curTri.longestSide();

    // 0 __d__ 1
    //   \ | /
    //    \|/
    //     2

    vec3 d = mix(curTri[side], curTri[side + 1],
                 Random::range(.5f - _skew, .5f + _skew));

    // ctx->line(d, curTri[side+2]);
    float newDepth = curTri.depth + Random::range(_decayMin, _decayMax);
    if (newDepth < _maxDepth) {
      ret.push_back(Tri{curTri[side], curTri[side + 2], d, newDepth});
      ret.push_back(Tri{curTri[side + 2], d, curTri[side + 1], newDepth});
    } else {
      auto sampColor = [=](vec3 pos) {
        return UI_Vertex{
            pos, mix(_minColor, _maxColor, noise.GetNoise(pos.x, pos.y))};
      };
      auto dsamp = sampColor(d);
      ctx->tri(sampColor(curTri[side + 2]), dsamp, sampColor(curTri[side + 1]));
      ctx->tri(sampColor(curTri[side]), sampColor(curTri[side + 2]), dsamp);
    }
    cur++;
  }
  return ret;
}

bool TriBuilder::imSettings() {
  bool redraw = false;
  redraw |= ImGui::InputInt("Seed", &_seed);
  redraw |= ImGui::SliderFloat("Max Depth", &_maxDepth, 0.0f, 12.0f);
  redraw |= ImGui::DragFloatRange2("Decay", &_decayMin, &_decayMax, .01f, .01f,
                                   _maxDepth);
  redraw |= ImGui::SliderFloat("Skew", &_skew, 0.f, .5f);
  redraw |= ImGui::SliderFloat("Frequency", &_frequency, 0.f, 100.f);
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
