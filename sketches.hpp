//
// Created by matt on 8/20/18.
//

#ifndef GL_PLAY_SKETCHES_HPP
#define GL_PLAY_SKETCHES_HPP

#include "Definitions.hpp"
#include "ref/FastNoise/FastNoise.h"


template<typename T>
vector<T> chaikin(vector<T> init, float smooth, float minDist){
  // http://graphics.cs.ucdavis.edu/education/CAGDNotes/Chaikins-Algorithm/Chaikins-Algorithm.html
  vector<T> seed = init;
  int maxIterations = 9; // todo: better heuristic, this will be ~ sizeof(init) * 512
  if(seed.size() == 0){ return {};}
  do {
    vector<T> output{ seed[0] };
    bool needed_cut = false;
    for(int i=0; i < seed.size() - 1; i++){
      if(glm::distance(seed[i], seed[i+1]) < minDist) { //i think it's fucky here?
        output.push_back( seed[i] );
        continue;
      };
      needed_cut = true;
      T q = glm::mix(seed[i], seed[i+1], smooth);
      T r = glm::mix(seed[i], seed[i+1], 1 - smooth);
      output.push_back( q );
      output.push_back( r );
    }
    if(!needed_cut){
      return output;
    }
    seed = output;
  } while(maxIterations-- > 0);
  return seed;
}

void do_curve(Processing* ctx, bool &_r, bool &_c, int curframe, int maxFrames){
  static int seed = 0;
  static int numPts = 5;
  static float minDist = .1;
  static float thickness = 0.002f;
  static int curLayer = 0;
  static int numLayers = 16;
  static float stepSize = .0003;
  static float noiseSize = 3;
  static int numSamples = 24;
  static float smoothing = .25;
  static float radius = 0.3;
  static FastNoise fnoise;

  static vector<vec3> line;

  bool redraw = false;

  STATIC_DO_ONCE(redraw = true;);

  // Typically we would use ImVec2(-1.0f,0.0f) to use all available width, or ImVec2(width,0.0f) for a specified width. ImVec2(0.0f,0.0f) uses ItemWidth.
  ImGui::ProgressBar((float) curLayer / numLayers, ImVec2(0.0f,0.0f));
  ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
  ImGui::Text("Progress Bar");
  redraw |= ImGui::InputInt("Seed", &seed);
  redraw |= ImGui::SliderFloat("radius", &radius, 0,1);
  redraw |= ImGui::SliderInt("numPts", &numPts, 2, 150);
  redraw |= ImGui::SliderFloat("minDist", &minDist, 0.001f, 2.f);
  redraw |= ImGui::SliderFloat("thickness", &thickness, 0.000001f, .1f);
  redraw |= ImGui::SliderFloat("sliderFloat", &smoothing, 0.01, 1.f);
  redraw |= ImGui::InputFloat("Step Size", &stepSize, 0.0001, 0.1);
  redraw |= ImGui::SliderFloat("NoiseSize", &noiseSize, 0, 1114);
  redraw |= ImGui::SliderInt("NumSamples", &numSamples, 2, 200);
  ImGui::SliderInt("Layers", &numLayers, 2, 400);

  if(!redraw && curLayer >= numLayers )
  {
    redraw = false;
    _r = false;
    _c = false;
    return;
  }

  curLayer++;
  if(redraw) {
    curLayer = 0;
  }
  ctx->clear();
  Random::seed(seed + curLayer);
  fnoise.SetSeed(seed+curLayer);

  if(redraw){
    line.clear();
    //Generate base line
    float di = 6.28f / (numPts - 1) ;
    for (int i = 0; i < numPts; i++) {
      vec3 newpt{sin(i*di) * radius, cos(i * di) * radius, 0};
      line.push_back(newpt);
    }
  }
  // moveDots
  for(int sample = 0; sample < numSamples; sample++) {
    for (int i = 0; i < line.size(); i++) {
      line[i].x +=  fnoise.GetNoise(line[i].x * noiseSize, line[i].y * noiseSize, (float) curLayer / numLayers * noiseSize) *         stepSize ;
      line[i].y +=  fnoise.GetNoise(line[i].x * noiseSize, line[i].y * noiseSize, 1290 + (float) curLayer / numLayers * noiseSize) *         stepSize ;
    }

    vector<vec3> pts = chaikin(line, smoothing, minDist);
    ctx->spline(pts, {1, 1, 1, 1}, {0, 0, 0, 0}, thickness);
  }
  ctx->flush();
  _r = true;
  _c = curLayer == 0;
  return ;
}

void spawnFlower(Processing *ctx) {

  const float pi = 3.1415f;

  static vec2 frequency{5, .1f};
  static float magnitude = 2;
  static int samples = 64;
  static int flowerSeed = 0;
  static float fullscale = 1;
  static float decay = .1f;
  static vec4 startColor{1, 0, 0, 0};
  static vec4 endColor{1, 1, 0, 1};

  bool redraw = false;
  redraw |= ImGui::InputInt("Seed", &flowerSeed);
  redraw |= ImGui::SliderFloat2("Frequency", (float *)&frequency, 1.f, 400.f);
  redraw |= ImGui::SliderFloat("Magnitude", &magnitude, 0.f, 10.f);
  redraw |= ImGui::SliderInt("Samples", &samples, 4, 256);
  redraw |= ImGui::SliderFloat("Decay", &decay, .01f, 1.f);
  redraw |= ImGui::SliderFloat("Fullscale", &fullscale, .001f, 2);
  if( ! redraw) return;
  ctx->clear();
  FastNoise noise;
  noise.SetSeed(flowerSeed);

  auto noisePoint = [=](float i, float theta) {
    vec3 pos{sin(theta), cos(theta),0};
    float r = i + decay * magnitude * (.5 - noise.GetNoise(pos.x * frequency[0] , pos.y * frequency[0], i * frequency[1]));
    return pos * r * fullscale;
  };
  auto noiseColor = [=](float i, float theta) {
    return mix(startColor, endColor, abs(sin(theta/2)+i));
  };

  float dTheta = 2 * pi / samples;
  UI_Vertex center{{0, 0, 0}, {0,0,0,0}};
  for (float i = 1; i > .001f; i -= decay) {
    for (float theta = 0; theta < (2 * pi); theta += dTheta) {
      auto a = noisePoint(i, theta);
      auto b = noisePoint(i, theta + dTheta);
      ctx->tri(UI_Vertex{a, noiseColor(i, theta)},
               UI_Vertex{b, noiseColor(i, theta + dTheta)}, center);
    }
  }
  ctx->flush();
}


void test_chaikin(){
  vector<vector<float>> params{
      {0, 16},
      {0,1,16},
      {0,1,16, 16},
      {0,0,1,4,7,9,11,3,444,16,16},
  };

  for(auto line : params){
    auto chain = chaikin(line, .25, 1);
    for(auto c : chain){
      cout<< c<<" ";
    }
    cout<<endl;
  }
}


#endif //GL_PLAY_SKETCHES_HPP

