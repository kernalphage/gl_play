//
// Created by matt on 6/17/18.
//

#ifndef GL_PLAY_FLAME_HPP
#define GL_PLAY_FLAME_HPP
#include "Definitions.hpp"

class Flame {
public:
  static vec2 linear(vec2 p, vec4 adj){ return p; }
  static vec2 sinus(vec2 p, vec4 adj){ return {sin(p.x * (1+adj.x) + adj.z), sin(p.y * (1+adj.y) + adj.w) };}
  static vec2 sphere(vec2 p, vec4 adj){
    float r = length(p - vec2(adj.x, adj.y));
    return 1.f / (r * r * (p + vec2(adj.x, adj.y)));
  }
  static vec2 swirl(vec2 p, vec4 adj){
    float r = length(p - vec2(adj.x, adj.y));
    r = r * r;
    return vec2(p.x * sin(r) - p.y * cos(r),  p.x* cos(r) + p.y * sin(r) );
  }

};


#endif //GL_PLAY_FLAME_HPP
