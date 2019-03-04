//
// Created by matt on 3/3/19.
//

#include "SlimeMold.hpp"
#include <iostream>
#include <Material.hpp>

#define numFeedbacks 2

void SlimeMold::imSettings(bool redraw, bool clear) {

}

void SlimeMold::setup() {


// FEEDBACK shader
  const GLchar* vertexShaderSrc = R"glsl(
  #version 330 core

    layout (location = 0) in vec4 i_position;
    layout (location = 1) in vec3 i_color;
    layout (location = 2) in float i_angle;

    out vec4 o_position;
    out vec3 o_color;
    out float o_angle;

    void main()
    {
        o_position = i_position + vec4(i_position.zw, 0,0);
        o_color = i_color;
        o_angle = 1;

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
  SlimeNode data[] = {{{0.0f, 0.0f, .01f, .0001f}, {1.0f, 0.0f, 1.0f}, .5f},
                      {{0.0f, .3f, -0.01f, -.0001f}, {0.0f, 1.0f, 1.0f}, .5f}
  };


    glGenTransformFeedbacks(2,m_transformFeedback);
    glGenBuffers(3,m_particlesBuffers);
    for (uint32_t n = 0;n < 2;n++)
    {
        glBindTransformFeedback(GL_TRANSFORM_FEEDBACK,m_transformFeedback[n]);
        glBindBuffer(GL_ARRAY_BUFFER,m_particlesBuffers[n]);
        glBufferData(GL_ARRAY_BUFFER,sizeof(data),data,GL_DYNAMIC_DRAW);

        glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER,0,m_particlesBuffers[n]);
     //   glBindBuffer(GL_ARRAY_BUFFER,0);
    }


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
  static int i = 0; 
  i = (i+1) % 2;
  int curVB = i;
  int curTFB = (i+1) %2;
  glPointSize(10);
  // 'draw' the buffer
  glUseProgram(m_program);

  glEnable(GL_RASTERIZER_DISCARD);
  glBindBuffer(GL_ARRAY_BUFFER,m_particlesBuffers[curVB]);
  glBindTransformFeedback(GL_TRANSFORM_FEEDBACK,m_transformFeedback[curTFB]);
  
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


    glBeginTransformFeedback(GL_POINTS);
static bool isFirst = true;
if(isFirst){
   glDrawArrays(GL_POINTS, 0, 2);
        isFirst = false;
  }
    else {
        glDrawTransformFeedback(GL_POINTS, m_transformFeedback[curVB]);
    } 
  // TODO: set uniforms 
    glEndTransformFeedback();
  glFlush();

  glDisable(GL_RASTERIZER_DISCARD);
  //Render particles from feedback object Current
 glBindBuffer(GL_ARRAY_BUFFER,m_particlesBuffers[curTFB]);
//  glEnableVertexAttribArray(0);
glDrawTransformFeedback(GL_POINTS,m_transformFeedback[curTFB]);
}

void SlimeMold::render(Processing *ctx, bool &redraw, bool &clear, int curFrame, int maxFrames) {

}


/*
void ParticleSystem::release()
{
    NvSafeDelete(m_feedbackProgram);
    NvSafeDelete(m_billboardProgram);
    NvSafeDelete(m_emitterProgram);
    
    glDeleteTextures(1,&m_randomTexture);
    glDeleteTextures(1,&m_particleTexture);
    glDeleteTextures(1,&m_FBMTexture);

    glDeleteTransformFeedbacks(countof(m_transformFeedback),m_transformFeedback);
    glDeleteBuffers(countof(m_particlesBuffers),m_particlesBuffers);
    glDeleteBuffers(1,&m_emittersBuffer);

    glDeleteQueries(countof(m_countQuery),m_countQuery);
}
*/