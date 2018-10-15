//
// Created by matt on 8/21/18.
//

#ifndef GL_PLAY_FLOWMAP_HPP
#define GL_PLAY_FLOWMAP_HPP
#include "Definitions.hpp"
#include "FastNoise/FastNoise.h"

class flowmap {
public:
  bool imSettings();
  void render(Processing* ctx);
  bool needsFrame();
private:

  template <typename T>
  vec3 compute_curl(vec3 pos, T turb);

  vec3 generate_point();
  vec3 next_point(const vec3 pt);
  bool validPoint(const int age, const vec3 curpt);
  void plot(const vec3 curpt, Processing* ctx);

  int seed = 125;
  int numpoints = 10;
  int numlayers = 400;
  int curlayer = 0;
  int maxAge = 40;

  float curl_epsilon = 0.001;

  float layerNoiseScale = 0.01;
  float sampleSize = .003;
  float speed = 0.002;
  float windSpeed = 0.001;
  FastNoise noise;
  float noise_frequency =2;
};


#endif //GL_PLAY_FLOWMAP_HPP
