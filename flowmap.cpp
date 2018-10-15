//
// Created by matt on 8/21/18.
//

#include <FastNoise/FastNoise.h>
#include "flowmap.hpp"
#include "imgui.h"
#include "Processing.hpp"
#include "Random.hpp"

bool flowmap::imSettings() {
  bool redraw = false;
  STATIC_DO_ONCE(redraw = true;);

  redraw |= ImGui::InputInt("Seed", &seed);

  redraw |= ImGui::SliderFloat("noiseFrequency", &noise_frequency, .2, 10);
  redraw |= ImGui::SliderInt("numPoints", &numpoints, 1,100);
  ImGui::SliderInt("numLayers", &numlayers, 1,1000);
  redraw |= ImGui::SliderFloat("sampleSize", &sampleSize, .001,.1);

  redraw |= ImGui::SliderFloat("speed", &speed, 0.001, .05);
  redraw |= ImGui::SliderFloat("windspeed", &windSpeed, 0.001, .05);
  redraw |= ImGui::SliderInt("maxAge", &maxAge, 10,1000);
  if(redraw){
    curlayer = 0;
  }
  return redraw;
}


void flowmap::render(Processing* ctx) {
  curlayer++;
  Random::seed( seed + curlayer);
  noise.SetSeed(seed);
  noise.SetFrequency( noise_frequency);
  ctx->clear();

  for(int i=0; i < numpoints; i++){
    vec3 curpt = generate_point();
    plot(curpt, ctx);
    int age= 0;
    while( validPoint(age, curpt) ) {
      auto nextpt = next_point(curpt);
      plot(nextpt, ctx);
      curpt = nextpt;
      age++;
    }
  }
  ctx->flush();
}


vec3 flowmap::next_point(const vec3 pt) {

  vec3 pp = vec3{abs(pt.x), pt.y, 0};

  return pt +
      /*compute_curl(pt, [&](const vec3 p){
    // TODO: give up and use textures
    return     */
      (vec3(pt.x -1, 0, 0) * windSpeed) + vec3{noise.GetSimplex(SPLAT3(pp), 1414),
                noise.GetSimplex(SPLAT3(pp), 4117),
               0,// noise.GetSimplex(SPLAT3(pp), 7112)
          } * speed;
    //}) * speed;
  //return pt + normalize(vec3{-pt.x, pt.y,0}) * speed;
}

vec3 flowmap::generate_point() {
  return vec3{Random::range(-1.f,1.f), Random::range(-1.f,1.f),  0};
}

bool flowmap::validPoint(const int age, const vec3 curpt) {
  //if(age == 0){
    return length(curpt) < .75  && age < maxAge;
  //}
}

void flowmap::plot(const vec3 curpt, Processing *ctx) {
  ctx->ngon(vec2{curpt.x, curpt.y}, sampleSize,6, {1,1,1,0}, {1,1,1,1} );
}

bool flowmap::needsFrame() {
  return curlayer <= numlayers;
}


template<typename T>
vec3 flowmap::compute_curl(vec3 p, T turb) {
  float e = curl_epsilon;
  vec3 dx= vec3(e, 0, 0);
  vec3 dy= vec3(0, e, 0);
  vec3 dz= vec3(0, 0, e);

  float x = turb(p+dy).z - turb(p-dy).z - turb(p+dz).y + turb(p-dz).y;
  float y = turb(p+dz).x - turb(p-dz).x - turb(p+dx).z + turb(p-dx).z;
  float z = turb(p+dx).y - turb(p-dx).y - turb(p+dy).x + turb(p-dy).x;

  return vec3(x * 2 * e, y * 2 * e, z * 2 * e);
};
