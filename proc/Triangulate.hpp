//
// Created by matt on 3/5/18.
//

#ifndef GL_PLAY_TRIANGULATE_HPP
#define GL_PLAY_TRIANGULATE_HPP
#include "Definitions.hpp"
#include "Processing.hpp"
#include <vector>
#include "Texture.hpp"

struct Tri {
  vec3 a, b, c;
  float depth;

  vec3 &operator[](size_t i) {
    i = i % 3;
    if (i == 0)
      return a;
    if (i == 1)
      return b;
    return c;
  };
  float area(){
    auto dab = glm::length2(a-b);
    auto dbc = glm::length2(b-c);
    auto dca = glm::length2(c-a);
    return (dab + dbc + dca) / 3;
  }
  vec3 center() const{
    return (a + b + c) * (1.0f/3.0f);
  }
  int longestSide();
};

struct TriBuilder {

  bool imSettings();

  std::vector<Tri> triangulate( Processing *ctx,int  curFrame);

  std::vector<Tri> _curTriangles = {Tri{{0, .8, 0}, {-.8, -.8, 0}, {.8, -.8, 0}, 0}};;
  int _seed;
  float _radius = .38f;
  float _frequency = 20;
  float _decayMin = .4f;
  float _decayMax = 1.5;
  float _maxDepth = 6;
  float _skew = .18f;
  float _linethickness = .001f;
  float _fillChance = 0.5f;
  bool _drawLeftParent = false;
  bool _drawRightParent = false;

  vec2 _resizeRange = {1.0f, 1.0f};
  vec4 _minColor{0.0f, 1.0f, 0.0f, .8f};
  vec4 _maxColor{1.0f};
  float _colorGain = 1.0f;
};

bool testTriangle();

#endif // GL_PLAY_TRIANGULATE_HPP
