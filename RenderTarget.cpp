#include "ref/glad/include/glad/glad.h"
#include "RenderTarget.hpp"
#include "Material.hpp"
#include "Definitions.hpp"
#define OFFSETOF(TYPE, ELEMENT) ((size_t)&(((TYPE *)0)->ELEMENT))
void RenderTarget::init(void)  {

  glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);

    // Color buffer
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, WIDTH, HEIGHT, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture, 0);

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
    glClearColor(0, 0, 0, 255);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  }
}

void RenderTarget::end() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

  m_twotri->use();
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, m_texture);
  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  glBindVertexArray(m_vao);
  glDrawArrays(GL_TRIANGLE_STRIP, 0,4);
  glBindVertexArray(0);

}

RenderTarget::RenderTarget(int _w, int _h) {
  m_twotri = new Material{"two_tri.vert", "two_tri.frag", false};
  WIDTH = _w;
  HEIGHT = _h;
}
