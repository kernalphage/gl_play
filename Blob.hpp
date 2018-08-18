//
// Created by matt on 3/29/18.
//

#ifndef GL_PLAY_BLOB_HPP
#define GL_PLAY_BLOB_HPP
#include "Definitions.hpp"
#include "Partition.hpp"
#include <vector>
#include <iostream>

using namespace std;

class Processing;

struct Blob{
  Blob(vec2 _pos, float _r): pos(_pos), r(_r){ }
  void render(Processing *ctx, vec4 color, float thickness);


  vec2 pos;
  float r;
  //int depth;
};


#endif //GL_PLAY_BLOB_HPP
