//
// Created by matt on 8/15/18.
//

#ifndef GL_PLAY_STREAMLINE_HPP
#define GL_PLAY_STREAMLINE_HPP

#include "Definitions.hpp"
#include "Partition.hpp"
#include <vector>
#include <numeric>
#include "Partition.hpp"
#include <FastNoise/FastNoise.h>

using namespace std;

class Streamline {
  struct Node{
    Node* next;
    vec2 pos;
    bool open;
  };

  struct Line{
    Node* root;
  };
public:

  Streamline(float _width, float _height):
      p{{0,0},{_width,_height}, _width/100},
      width(_width), height(_height)
  {
    p.resize({-1,-1}, {2,2}, stepDist * distFactor);
  };
  ~Streamline(){
    destroy();
  }

  bool imSettings();

  bool isvalid(vec2 pt, Node* prev = nullptr);

  vec2 next(Node* pos, bool backwards);
  Node* stream_point(vec2 startPos);

  void render(Processing* ctx);

  vec2 nextOpenPoint();

private:
  void destroy();
  template<typename T>
  void compute_curl(vec2 pos, T turbulence);

  vector<Node*> lines;
  vector<Node*> backlines;
  Partition<Node*> p;
  float width;
  float height;
  float distFactor = .5f;

  int seed =0 ;
  float lineWidth=.004;
  float stepDist = .0113;
  float windStrength = .2;
  int maxIterations = 100;
  FastNoise f;
};


#endif //GL_PLAY_STREAMLINE_HPP
