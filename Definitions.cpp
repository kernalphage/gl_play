//
// Created by matt on 3/12/19.
//
#include <Definitions.hpp>
#include <fmt/format.h>

const int Util::numbers[] = {48,49,50,51,52,53,54,55,56,57,65,75,81,74,9824,9825,9826,9827,9828,9829,9830};
Texture Util::distField[DEPTH_MAP_COUNT];
//Processing* Util::debug_ctx;

float Util::rangeMap(float t, float inStart, float inEnd, float outStart, float outEnd, bool doClamp) {
  float out = t - inStart;
  out /= (inEnd - inStart); // [0,1]
  // TODO: warp/reflect enum?
  if(doClamp){
    out = glm::clamp(out, 0.0f,1.0f);
  }
  out *= (outEnd - outStart);
  return out + outStart;
}

std::string Util::timestamp(int seed) {
  char mbstr[150];
  std::time_t t = std::time(nullptr);
  std::strftime(mbstr, sizeof(mbstr), "%m_%d_%s_%%d.txt", std::localtime(&t));
  sprintf(mbstr, mbstr, seed);
  return std::string(mbstr);
}



bool Util::initUtilities() {
  static bool doneOnce= false;
  if(doneOnce) { return true;}
    doneOnce = true;
    // initialize distance fields
    for(int i=0; i < DEPTH_MAP_COUNT; i++){
      auto filename = fmt::format("sdf_text/{0}_fixed.png", numbers[i]);
      distField[i].uploadToGPU = false;
      distField[i].load(filename.c_str());
    }

//  debug_ctx = new ProcessingGL_t<UI_Vertex, vec4>{};
  return true;
}
vec4 Util::sampleDistField(vec2 ppos, int idx) {
  idx %= DEPTH_MAP_COUNT;
  return vec4( distField[idx].sample(ppos.x, ppos.y, 0) ,
               distField[idx].sample(ppos.x, ppos.y, 1) ,
               distField[idx].sample(ppos.x, ppos.y, 2) ,
               distField[idx].sample(ppos.x, ppos.y, 3) );
}
