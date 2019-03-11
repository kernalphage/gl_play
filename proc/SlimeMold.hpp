//
// Created by matt on 3/3/19.
//

#ifndef GL_PLAY_SLIMEMOLD_HPP
#define GL_PLAY_SLIMEMOLD_HPP
#include <Definitions.hpp>



class Material; 
struct SlimeNode{
  vec3 position;
  vec4 color;
  float angle;
};

#define SLIME_ROWS 4
#define SLIME_NODES  SLIME_ROWS * SLIME_ROWS
#define SLIME_TOTAL_SIZE SLIME_NODES * 6

class SlimeMold {
public:
  void setup();
  void imSettings(bool redraw, bool clear);
  void render(Processing* ctx, bool& redraw, bool& clear, int curFrame, int maxFrames);
  void feedbackStep();
private:
  void setVertexArrays();
  void setDrawArrays();
  GLuint m_particlesBuffers[3];
  GLuint m_transformFeedback[3];
  GLuint program;
  GLint uniTime;
  GLuint vbo, tbo;
GLint uniMousePos;

  GLfloat data[SLIME_TOTAL_SIZE] = {0};

  Material* basic;
};


#endif //GL_PLAY_SLIMEMOLD_HPP
