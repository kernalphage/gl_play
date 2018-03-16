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

  static std::vector<Tri> triangulate(const std::vector<Tri>& seed, float maxDepth, Processing* ctx);
  static int longestSide(Tri t);


};



bool testTriangle();

#endif //GL_PLAY_TRIANGULATE_HPP
