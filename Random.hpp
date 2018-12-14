#ifndef RANDOM_HPP
#define RANDOM_HPP
#include <random>
#include "Definitions.hpp"

class Random {
  static std::default_random_engine generator;
  static std::uniform_real_distribution<float> distribution;
  static std::normal_distribution<float> gauss_dist;

public:
  static void seed(int s = 0) {
    generator = std::default_random_engine{};
    generator.seed(s);
    distribution = std::uniform_real_distribution<float>{0.0f, 1.0f};
    gauss_dist = std::normal_distribution<float>{0.0f, 1.0f};
    
  }
  static float f() { return distribution(generator); }
  static float nextGaussian(){return gauss_dist(generator); }
  static vec2 gaussPoint(){return vec2{gauss_dist(generator), gauss_dist(generator)}; }
  static int range(int a, int b) {
    return (int)(a + (b - a) * distribution(generator));
  }
  static float range(float a, float b) {
    return a + (b - a) * distribution(generator);
  }

  static vec2 random_point(vec2 tl, vec2 br) {
    return {Random::range(tl.x, br.x),
            Random::range(tl.y, br.y)};
  }
  static vec3 random_point(vec3 tl, vec3 br) {
    return {Random::range(tl.x, br.x),
            Random::range(tl.y, br.y),
            Random::range(tl.z, br.z)};
  }  
  static vec4 random_point(vec4 tl, vec4 br) {
    return {Random::range(tl.x, br.x),
            Random::range(tl.y, br.y),
            Random::range(tl.z, br.z),
            Random::range(tl.w, br.w)};
  }

};

#endif