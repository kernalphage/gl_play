//
// Created by matt on 6/17/18.
//

#ifndef GL_PLAY_FLAME_HPP
#define GL_PLAY_FLAME_HPP
#include "Definitions.hpp"


class Flame {
public: 
  void randomInit();
  
  bool imSettings(int id);
  vec2 fn(vec2 pt);
  int type;
  vec2 offset{0,0};
  vec4 vars{0,0,0,0};
  float strength = .5;

  using FolderFunction = vec2 (*)(const vec2,const vec4);
private:


public: // Static
  static vec2 linear(const vec2 p, const vec4 adj){ 
    return p; //TODO: change to 3x3 matrix?;
   }
  static vec2 sinus(const vec2 p, const vec4 adj){ return {sin(p.x * (1+adj.x) + adj.z), sin(p.y * (1+adj.y) + adj.w) };}
  static vec2 sphere(const vec2 p, const vec4 adj){
    float r = length(p - vec2(adj.x, adj.y));
    return 1.f / (r * r * (p + vec2(adj.x, adj.y)));
  }
  static vec2 swirl(const vec2 p, const vec4 adj){
    float r = length(p - vec2(adj.x, adj.y));
    r = r * r;
    return vec2(p.x * sin(r) - p.y * cos(r),  p.x* cos(r) + p.y * sin(r) );
  }
  static vec2 horseshoe(const vec2 p, const vec4 adj){
    float r = length(p - vec2(adj.x, adj.y));
    return (1/r) * vec2((p.x-p.y)*(p.x+p.y), 2*p.x*p.y);
  }
  static vec2 polar(const vec2 p, const vec4 adj){
    vec2 offset = p - vec2(adj.x, adj.y);
    float r = length(offset);
    float theta = atan2(offset.y,offset.x);
    return vec2(theta / 3.141, r - 1);
  }
  static vec2 handkerchief(const vec2 p, const vec4 adj){
    vec2 offset = p - vec2(adj.x, adj.y);
    float r = length(offset);
    float theta = atan2(offset.y,offset.x);
    return vec2(sin(theta + r + adj.z), cos(theta - r + adj.w));
  }
  static vec2 heart(const vec2 p, const vec4 adj){
    return p;
  }
};


#endif //GL_PLAY_FLAME_HPP
