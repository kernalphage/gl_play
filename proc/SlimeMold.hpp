//
// Created by matt on 3/3/19.
//

#ifndef GL_PLAY_SLIMEMOLD_HPP
#define GL_PLAY_SLIMEMOLD_HPP
#include <Definitions.hpp>

struct SlimeNode{
  vec4 position;
  vec3 color;
  float angle;
};

class SlimeMold {
public:
  void setup();
  void imSettings(bool redraw, bool clear);
  void render(Processing* ctx, bool& redraw, bool& clear, int curFrame, int maxFrames);
  void feedbackStep();
private:
  GLuint m_particlesBuffers[3];
  GLuint m_transformFeedback[3];
  GLuint m_program;
};


#endif //GL_PLAY_SLIMEMOLD_HPP
