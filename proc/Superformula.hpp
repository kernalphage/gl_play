//
// Created by matt on 1/1/19.
//

#ifndef GL_PLAY_SUPERFORMULA_HPP
#define GL_PLAY_SUPERFORMULA_HPP
#include <Definitions.hpp>
#include "imgui.h"

class Superformula{
public:
  static void drawVortex(Processing* ctx, bool& redraw, bool& clear, int curFrame, int maxFrames) {
    STATIC_DO_ONCE(clear = true);
    static float a = .315f,b = .152f,m = .1245f;
    static vec3 n{.2,.5,.5};
    static int numPts;

    ImGui::SliderFloat("a", &a, 0, 3);
    ImGui::SliderFloat("b", &b, 0, 3);
    ImGui::SliderFloat("m", &m, 0, 3);

    ImGui::SliderFloat3("n", &(n.x), 0, 1);
    ImGui::SliderInt("numpts", &numPts, 0, 500);
    auto super = [](float t){
      float m4t = m / 4.0f * t;
      float cosPart = pow(glm::abs( cos(m4t) / a), n.y);
      float sinPart= pow(glm::abs( sin(m4t) / b), n.z);
      return vec2{sin(t), cos(t)} * pow(cosPart + sinPart, -pow(n.x, -1.0f));
    };
    float di = 6.28f/numPts;

    for(int i=0; i < numPts; i++){
      vec2 pos = super(i*di);
      ctx->ngon(pos, 0.04, 6, vec4{SPLAT2(abs(pos)), 0, 0}, vec4{SPLAT2(abs(pos)), 0, 1000});
    }
    redraw = true;
    clear = true;
  };

};
#endif //GL_PLAY_SUPERFORMULA_HPP
