//
// Created by matt on 3/5/18.
//

#ifndef GL_PLAY_TRIANGULATE_HPP
#define GL_PLAY_TRIANGULATE_HPP
#include "Definitions.h"
#include "Processing.h"
#include <vector>

struct Tri{
  vec3 a, b, c;
  float depth;

  vec3& operator[](size_t i){
    i = i % 3;
    if(i==0) return a;
    if(i==1) return b;
    return c;
  };
	int longestSide();

};

struct TriBuilder{

  bool imSettings();

 std::vector<Tri> triangulate(const std::vector<Tri>& seed, Processing* ctx);
 float _decayMin = 1;
 float _decayMax = 1;
 float _maxDepth = 3;
 float _skew = .1f;
};

bool testTriangle();

#endif //GL_PLAY_TRIANGULATE_HPP
