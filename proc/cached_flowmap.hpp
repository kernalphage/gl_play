//
// Created by matt on 12/31/18.
//

#ifndef GL_PLAY_CACHED_FLOWMAP_HPP
#define GL_PLAY_CACHED_FLOWMAP_HPP
#include "Definitions.hpp"
#include "Random.hpp"
#include "RandomCache.hpp"
#include "FastNoise/FastNoise.h"
#include "imgui.h"

class cached_flowmap {
public:

  static void drawVortex(Processing* ctx, bool& redraw, bool& clear, int curFrame, int maxFrames) {
    STATIC_DO_ONCE(clear = true);
    static FastNoise n;
    static RandomCache<vec2> randMap{1000, 2, [](vec2 pos){return vec2(n.GetValueFractal(pos.x, pos.y, 0),
                                                                       n.GetValueFractal(pos.x, pos.y, 100));}};
    static float noisefreq = 8.31511;
    static int numpts = 10;
    static float maxVel = .003;
    static float angleVel = 1.451;
    static float noiseScale = .2f;
    static int numIterations = 1155;
    static int curIterations = 0;
    static int totalIterations = 1000;
    static float curlStrength = .2;
    static float swirlStrength = .2;
    static float globeSize = 0.1;
    static float tendrilStrength = .4;
    static float deadzone = 5;
    ImGui::ProgressBar((float) curIterations / totalIterations, ImVec2(0.0f,0.0f));
    ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
    ImGui::Text("frame progress");
    bool reNoise = ImGui::SliderFloat("frequency", &noisefreq, .01, 35);
    reNoise |= ImGui::SliderFloat("curlStrength", &curlStrength, 0.01, 1);
    reNoise |= ImGui::SliderFloat("swirlStrength", &swirlStrength, 0.01, 1);
    reNoise |= ImGui::SliderFloat("tendrilStrength", &tendrilStrength, 0.01, 1);
    reNoise |= ImGui::SliderFloat("desdzone", &deadzone, 0, 5);
    STATIC_DO_ONCE(reNoise = true);
    reNoise |= ImGui::SliderFloat("timeScale", &noiseScale, 0.01, 1);

    if(reNoise ){
      //totalTime += .01;
      const float totalTime = (float) 6.28f * curFrame / maxFrames;
      const float sT = sin(totalTime)*noiseScale;
      const float cT = cos(totalTime)*noiseScale;
      n.SetFrequency(noisefreq);

      auto vortex = [=](vec2 pos){
        pos = pos - vec2(.5f,.5f);
        auto curl_noise = vec2(n.GetValueFractal(pos.x, pos.y + sT, cT),
                               n.GetValueFractal(pos.x, pos.y + sT, cT + 100))* curlStrength ;
        auto swirl = vec2(-pos.y, pos.x) * swirlStrength;

        float intensity = (1 - pos.x * pos.x) * glm::sign(pos.y) * glm::min(pos.y * pos.y *  deadzone, 1.0f);
        auto tendril = vec2(-pos.x, intensity) * tendrilStrength;
        return swirl + curl_noise + tendril;
      };
      auto wireframe = [=](vec2 f){vec2 norm = glm::normalize(f); return vec2(glm::round(norm.x), glm::round(norm.y)) * glm::length(f);};
      randMap.reseed(1, vortex);
      clear = true;
    }
    ImGui::LabelText("cur iterations", "cur iterations %d", curIterations);
    clear |= ImGui::SliderFloat("velocity", &maxVel, 0,.1);
    clear |= ImGui::SliderFloat("globeSize", &globeSize, 0,.5f);
    ImGui::SliderInt("numPts", &numpts, 10, 500);
    ImGui::SliderInt("num iterations", &numIterations, 1, 10000);

    redraw = true;
    for (int i = 0; i < numpts; i++) {

      vec2 pos =  Random::gaussPoint() * globeSize;
      vec4 color = lerp(vec4{99,112,110,2400}/255.0f, vec4{0,229,255,2400}/255, abs(pos.x));
      for(int iter = 0; iter < numIterations; iter++) {

        auto force = randMap.sample(pos * .5 + vec2(.5f, .5f)) * maxVel;
        pos += force;
        // TODO: base radius on distance, fake depth of field
        ctx->ngon(pos, glm::max(glm::length(force), maxVel/2), 6, vec4{SPLAT3(color), 0}, color);
      }
    }
  }
};


#endif //GL_PLAY_CACHED_FLOWMAP_HPP
