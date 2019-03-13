//
// Created by matt on 3/29/18.
//

#ifndef GL_PLAY_BLOB_HPP
#define GL_PLAY_BLOB_HPP
#include "Definitions.hpp"
#include "Partition.hpp"
#include <vector>
#include <iostream>
#include <Texture.hpp>

using namespace std;


struct Blob{
  Blob(vec2 _pos, float _r): pos(_pos), r(_r){ }
  void render(Processing *ctx, vec4 color, float thickness);

  vec2 pos;
  float r;
  //int depth;
};


class BlobPlacer{
public:
  void render(Processing* ctx);
  void imSettings(bool& redraw, bool& clear);

  typedef std::function<vec2(vec2)> DistanceFN;
  void gen_poisson(vec2 tl, vec2 br, DistanceFN distFN, int maxSamples, vector<Blob> &out, float overlap);

private: 
	float _minRadius = .01;
	float _maxRadius = .1;
  float _radiusSkew = .5;
  float _overlap = .01f;
	int _maxSamples = 200;
	int _seed = 0;
  int _maxTextDist = 100;
  // sue me harder
  const int numbers[14] = {48,49,50,51,52,53,54,55,56,57,65,75,81, 74};
  Texture distField[14] ;
};

#endif //GL_PLAY_BLOB_HPP
