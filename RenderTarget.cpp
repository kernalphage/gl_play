#include "ref/glad/include/glad/glad.h"
#include "RenderTarget.hpp"
#include "Material.hpp"
#include "Definitions.hpp"

#include "imgui.h"

#define OFFSETOF(TYPE, ELEMENT) ((size_t)&(((TYPE *)0)->ELEMENT))
void RenderTarget::init(void)  {

  glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);

    // Color buffer
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, WIDTH, HEIGHT, 0, GL_RGB, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture, 0);


     glEnable(GL_BLEND);
     glBlendFunc(GL_SRC_ALPHA, GL_ONE);
   //  glClearColor(0,0,0,1);
  //  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

  vec4 verts[] = {{ -.9f,  .9f,  0,  1,},
                  {  .9f,  .9f,  1,  1,},
                  { -.9f, -.9f,  0,  0,},
                  {  .9f, -.9f,  1,  0,},
  };
  // Data for those sweet, sweet two triangles
  glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    glUseProgram(m_twotri->ID);


  m_twotri->setInt("renderedTexture", 0);
  m_twotri->setInt("tonemap", 1);

// Linking shader attributes to cpp structs
    GLint posAttrib = glGetAttribLocation(m_twotri->ID, "vertex_pos");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE,
                          sizeof(glm::vec4), (void*) 0);

    GLint texAttrib = glGetAttribLocation(m_twotri->ID, "vertex_uv");
    glEnableVertexAttribArray(texAttrib);
    glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE,
                          sizeof(glm::vec4), (void*) OFFSETOF(vec4, z));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


#if FFMPEG
    ffmpeg_encoder_start("tmp.mpg", AV_CODEC_ID_MPEG1VIDEO, 25, WIDTH, HEIGHT);
#endif
}

void RenderTarget::deinit() {
    glDeleteFramebuffers(1, &m_fbo);
}

void RenderTarget::begin(bool clear) {

    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glViewport(0,0,WIDTH,HEIGHT);
  if(clear) {
     glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  }

  ImGui::SliderFloat("Gamma", &gamma, 0,1.0);
  ImGui::SliderFloat("Energy", &energy, 1, 100);

  m_twotri->setFloat("gamma", gamma);
  m_twotri->setFloat("energy", energy);

}

void RenderTarget::end() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

  m_twotri->use();
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, m_texture);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, m_tonemap);

  
  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  glBindVertexArray(m_vao);
  glDrawArrays(GL_TRIANGLE_STRIP, 0,4);
  glBindVertexArray(0);

}

RenderTarget::RenderTarget(int _w, int _h) {
  m_twotri = new Material{"shaders/two_tri.vert", "shaders/two_tri.frag", false};
  WIDTH = _w;
  HEIGHT = _h;


  // Start texture load
  pixels = stbi_load("tonemap.png", &texWidth, &texHeight, &texChannels, 0);
  glGenTextures(1, &m_tonemap);
  glBindTexture(GL_TEXTURE_2D, m_tonemap);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texWidth, texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
}
