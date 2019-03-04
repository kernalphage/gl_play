//
// Created by matt on 3/3/19.
//

#include "SlimeMold.hpp"
#include <iostream>
#include <Material.hpp>


void SlimeMold::imSettings(bool redraw, bool clear) {

}

void SlimeMold::setup() {


// FEEDBACK shader
  const GLchar* vertexShaderSrc = R"glsl(
  #version 330 core

    in vec4 i_position;
    in vec3 i_color;
    in float i_angle;

    out vec4 o_position;
    out vec3 o_color;
    out float o_angle;

    void main()
    {
        o_position = i_position + vec4(i_position.zw, 0,0);
        o_color = i_color;
        o_angle = i_angle;

        gl_Position = vec4(o_position.xy, 0.0, 1.0);
    }
)glsl";

  const GLchar* fragShaderSrc = R"glsl(
  #version 330 core
  layout(location = 0) out vec4 color;
  in vec3 o_color;

  void main()
  {
      color = vec4(o_color, 1.0);
  }
)glsl";

  bool ok = true;
  m_program = glCreateProgram();

  GLuint shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(shader, 1, &vertexShaderSrc, nullptr);
  glCompileShader(shader);
  glAttachShader(m_program, shader);

  Material::checkCompileErrors(shader, "VERT", "slimeMold.vert");
  shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(shader, 1, &fragShaderSrc, nullptr);
  glCompileShader(shader);
  glAttachShader(m_program, shader);
  Material::checkCompileErrors(shader, "FRAG", "slimeMold.FRAG");

// maps vertex out back to a buffer
  const GLchar* feedbackVaryings[] = {"o_position", "o_color","o_angle"};
  glTransformFeedbackVaryings(m_program, 3, feedbackVaryings, GL_INTERLEAVED_ATTRIBS);

  glLinkProgram(m_program);
  glUseProgram(m_program);
  Material::checkCompileErrors(m_program, "PROGRAM", "slimeMold program");

  GLuint vao;
  glGenVertexArrays(1,&vao);
  glBindVertexArray(vao);

  // generate pinpong buffers and input data
// input data
  SlimeNode data[] = {{{0.0f, 0.0f, 0.01f, .0001f}, {1.0f, 0.0f, 1.0f}, .5f},
                      {{0.0f, .3f, -0.01f, -.0001f}, {0.0f, 1.0f, 1.0f}, .5f}
  };

  glGenBuffers(1, &m_prev);
  glBindBuffer(GL_ARRAY_BUFFER, m_prev);
  glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STREAM_DRAW);

  glGenBuffers(1, &m_cur);
  glBindBuffer(GL_ARRAY_BUFFER, m_cur);
  glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STREAM_DRAW);

  auto nodeSize= sizeof(SlimeNode);
  GLint inputAttrib = glGetAttribLocation(m_program, "i_position");
  glEnableVertexAttribArray(inputAttrib);
  glVertexAttribPointer(inputAttrib, 4, GL_FLOAT, GL_FALSE, nodeSize,0);

   inputAttrib = glGetAttribLocation(m_program, "i_color");
  glEnableVertexAttribArray(inputAttrib);
  glVertexAttribPointer(inputAttrib, 1, GL_FLOAT, GL_FALSE, nodeSize,(void*) sizeof(vec4));

   inputAttrib = glGetAttribLocation(m_program, "i_angle");
  glEnableVertexAttribArray(inputAttrib);
  glVertexAttribPointer(inputAttrib, 1, GL_FLOAT, GL_FALSE, nodeSize,(void*) (sizeof(vec4) + sizeof(vec3)));

}
void SlimeMold::feedbackStep() {
  glUseProgram(m_program);
  glPointSize(10);
  // 'draw' the buffer
  glEnable(GL_RASTERIZER_DISCARD);
 glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, m_cur);
  glBeginTransformFeedback(GL_POINTS);
  // these next two line are possibly equivalent to
  glDrawTransformFeedback(GL_POINTS, m_prev);
  //glBindBuffer(GL_ARRAY_BUFFER, m_prev);
  //glDrawArrays(GL_POINTS, 0, 2);

  glEndTransformFeedback();
  glDisable(GL_RASTERIZER_DISCARD);

  glFlush();

  //Render particles from feedback object Current
  glDrawTransformFeedback(GL_POINTS, m_cur);

  SlimeNode feedback[2];
  glGetBufferSubData(GL_TRANSFORM_FEEDBACK, 0, sizeof(feedback), feedback);
  std::cout<<glm::to_string(feedback[0].position) << " , " <<glm::to_string(feedback[1].position)<<std::endl;

  //Swap feedback objects
  auto tmp = m_cur;
  m_cur =  m_prev;
  m_prev = tmp;
}

void SlimeMold::render(Processing *ctx, bool &redraw, bool &clear, int curFrame, int maxFrames) {

}
