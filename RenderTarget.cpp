#include "ref/glad/include/glad/glad.h"
#include "RenderTarget.hpp"
#include "Material.hpp"
#include "Definitions.hpp"

#include "imgui.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

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


  glBindFramebuffer(GL_FRAMEBUFFER, 0);
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

  ImGui::LabelText("Render Target", "Render Target");
  ImGui::SliderFloat("Gamma", &gamma, 0,1.0);
  ImGui::SliderFloat("Energy", &energy, 1, 30);
  ImGui::SliderFloat("texAmt", &texAmt, 0.0f, 1.0f);

  m_twotri->setFloat("gamma", gamma);
  m_twotri->setFloat("energy", energy);

  m_twotri->setFloat("texAmt", texAmt);
}

void RenderTarget::end() {
  glFinish();
  if(ImGui::Button("Save")){
    std::string filename = "output/Render_capture" + Util::timestamp(0) + ".png";
    save(filename.c_str());
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  m_twotri->use();
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, m_texture);
  m_tonemap.use(GL_TEXTURE1);

  
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
  m_tonemap.load("tonemap.png");
}

void RenderTarget::save(const char *const filename) {


  float* highres_pixels = new float[WIDTH*HEIGHT*4];
  unsigned char* mapped_pixels = new unsigned char[WIDTH*HEIGHT*4];
  glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
  glViewport(0,0,WIDTH,HEIGHT);
  glReadPixels(0,0,WIDTH, HEIGHT, GL_RGBA, GL_FLOAT, highres_pixels);
  int dirty_pixels = 0;
  for(int i=0; i < WIDTH; i++){
    for(int j=0; j < HEIGHT; j++){
        const int idx = (j * WIDTH + i) * 4 ;

        vec4 lum = vec4(highres_pixels[idx], highres_pixels[idx+1], highres_pixels[idx+2], highres_pixels[idx+3]);
        lum = log(lum) * gamma;
        lum = glm::clamp(lum, 0.0f, 1.0f);
        for(int i=0; i < 4; i++)
        {
          lum[i] = pow(lum[i], energy);
        }
        auto toneSample = vec4(m_tonemap.sample(lum.x, lum.y,0), m_tonemap.sample(lum.x, lum.y,1), m_tonemap.sample(lum.x, lum.y,2), m_tonemap.sample(lum.x, lum.y,3));

        lum = glm::lerp(lum * 255, toneSample, texAmt);
//        mapped_pixels[idx] = (unsigned char) (lum * 255);
      mapped_pixels[idx+0] = (unsigned char)(lum.x);
      mapped_pixels[idx+1] = (unsigned char)(lum.y);
      mapped_pixels[idx+2] = (unsigned char)(lum.z);
      mapped_pixels[idx+3] = (unsigned char)(lum.w);
/*
        const int idx = (j * WIDTH + i) * 4;
        float numHits = highres_pixels[idx + 3];
        float lum = log(numHits) * gamma;
        lum = glm::clamp(lum, 0.0f, 1.0f);
        lum = pow(lum, energy);
        mapped_pixels[idx + 0] = (unsigned char) (glm::clamp(highres_pixels[idx + 0] / numHits, 0.0f, 1.0f) * 255);
        mapped_pixels[idx + 1] = (unsigned char) (glm::clamp(highres_pixels[idx + 1] / numHits,0.0f, 1.0f) * 255);
        mapped_pixels[idx + 2] = (unsigned char) (glm::clamp(highres_pixels[idx + 2] / numHits,0.0f,1.0f) * 255);
        mapped_pixels[idx + 3] = (unsigned char) (lum * 255);
        */
      mapped_pixels[(j * WIDTH + i) * 4 + 3] = (unsigned char) ( 255);
    }
  }

  int ok = stbi_write_png(filename,WIDTH, HEIGHT, 4, mapped_pixels, WIDTH*4);

  printf("render %s Ok: %d with %d dirty pixels \n", filename, ok, dirty_pixels);

  delete[](highres_pixels);
  delete[](mapped_pixels);
}


