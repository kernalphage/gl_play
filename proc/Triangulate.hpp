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

  DONER_DECLARE_OBJECT_AS_REFLECTABLE(TriBuilder)

  bool imSettings();

  std::vector<Tri> triangulate( Processing *ctx,int  curFrame);

  std::vector<Tri> _curTriangles = {Tri{{0, .8, 0}, {-.8, -.8, 0}, {.8, -.8, 0}, 0}};;
  int _seed = 10;
  float _maxDepth = 14.53;
  float _decayMin = 1.209f;
  float _decayMax = 1.39;
  float _skew = .083f;
  vec2 _resizeRange = {0.988f, 1.02f};
  float _radius = .452f;
  float _fillChance = 0.640f;
  vec4 _minColor{0.0f, 1.0f, 0.0f, .8f};
  vec4 _maxColor{1.0f};
  float _colorGain = 1.521f;
  float _linethickness = .001f;
  bool _drawLeftParent = true;
  bool _drawRightParent = true;

};

DONER_DEFINE_REFLECTION_DATA(TriBuilder,
      DONER_ADD_NAMED_VAR_INFO(_seed, "_seed"),
        DONER_ADD_NAMED_VAR_INFO(_frequency, "_frequency"),
      DONER_ADD_NAMED_VAR_INFO(_decayMin, "_decayMin"),
      DONER_ADD_NAMED_VAR_INFO(_decayMax, "_decayMax"),
      DONER_ADD_NAMED_VAR_INFO(_maxDepth, "_maxDepth"),
      DONER_ADD_NAMED_VAR_INFO(_skew, "_skew"),
      DONER_ADD_NAMED_VAR_INFO(_linethickness, "_linethickness"),
      DONER_ADD_NAMED_VAR_INFO(_fillChance, "_fillChance"),
      DONER_ADD_NAMED_VAR_INFO(_drawLeftParent, "_drawLeftParent"),
      DONER_ADD_NAMED_VAR_INFO(_drawRightParent, "_drawRightParent")

)


bool testTriangle();

#endif // GL_PLAY_TRIANGULATE_HPP
