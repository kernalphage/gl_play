//
// Created by matt on 3/5/18.
//

#ifndef GL_PLAY_TRIANGULATE_HPP
#define GL_PLAY_TRIANGULATE_HPP
#include "Definitions.hpp"
#include "Processing.hpp"
#include <vector>

struct Tri {
  vec3 a, b, c;
  float depth;
  vec4 ca, cb, cc;

  vec3 &operator[](size_t i) {
    i = i % 3;
    if (i == 0)
      return a;
    if (i == 1)
      return b;
    return c;
  };
  vec3 center() const{
    return (a + b + c) * (1.0f/3.0f);
  }
  int longestSide();
};

struct TriBuilder {

  bool imSettings();

  std::vector<Tri> triangulate(float depthMod, Processing *ctx);

  std::vector<Tri> _curTriangles = {Tri{{0, .8, 0}, {-.8, -.8, 0}, {.8, -.8, 0}, 0}};;
  int _seed;
  float _radius = .68f;
  float _frequency = 20;
  float _decayMin = .6f;
  float _decayMax = 1;
  float _maxDepth = 6;
  float _zoom = 0;
  float _skew = .18f;
  vec4 _minColor{0.0f, 1.0f, 0.0f, .8f};
  vec4 _maxColor{1.0f};
};

bool testTriangle();

#endif // GL_PLAY_TRIANGULATE_HPP
