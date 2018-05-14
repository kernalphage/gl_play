//
// Created by matt on 4/28/18.
//

#include "RenderTarget.hpp"

RenderTarget::RenderTarget(int width, int height)
  :m_width(width), m_height(height)
{
  glGenFramebuffers(1, &frameBuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

  glGenTextures(1, &renderTexture);
  glBindTexture(GL_TEXTURE_2D, renderTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

  // Poor filtering. Needed !
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  // The depth buffer
  glGenRenderbuffers(1, &depthrenderbuffer);
  glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);


  // Set "renderedTexture" as our colour attachement #0
  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderTexture, 0);

// Set the list of draw buffers.
  GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
  glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers


  // Always check that our framebuffer is ok
  if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
  {
    printf("ERROR: Rendertarget %d failed to bind", frameBuffer);
  }
  else{
    printf("working fine my dude");
  }

  // The fullscreen quad's FBO
  static const GLfloat g_quad_vertex_buffer_data[] = {
      -1.0f, -1.0f, 0.0f,
      1.0f, -1.0f, 0.0f,
      -1.0f,  1.0f, 0.0f,
      -1.0f,  1.0f, 0.0f,
      1.0f, -1.0f, 0.0f,
      1.0f,  1.0f, 0.0f,
  };

  glGenBuffers(1, &quad_vertexbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data, GL_STATIC_DRAW);
  glVertexAttribPointer(
      0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
      3,                  // size
      GL_FLOAT,           // type
      GL_FALSE,           // normalized?
      0,                  // stride
      (void*)0            // array buffer offset
  );


  mat = std::make_unique<Material>("two_tri.vert", "two_tri.frag", false);
  mat.get()->use();
  texID  = glGetUniformLocation(mat.get()->ID, "renderedTexture");

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderTarget::activate() {
  glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
  glViewport(0,0, m_width, m_height);
  // Clear the screen
  //glClear(GL_DEPTH_BUFFER_BIT); //| GL_COLOR_BUFFER_BIT
}

void RenderTarget::render() {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glViewport(0,0, m_width, m_height);
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  mat.get()->use();

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, renderTexture);
  glUniform1i(texID, 0);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
  // Draw the triangles !
  glDrawArrays(GL_TRIANGLES, 0, 6); // 2*3 indices starting at 0 -> 2 triangles

}

RenderTarget::~RenderTarget() {
  glDeleteBuffers(1, &frameBuffer);
  glDeleteTextures(1, &renderTexture);
  glDeleteRenderbuffers(1, &depthrenderbuffer);
  glDeleteVertexArrays(1, &quad_VertexArrayID);

}
