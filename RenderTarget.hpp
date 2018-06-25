#ifndef RENDER_TARGET_HPP
#define RENDER_TARGET_HPP

#include <stb/stb_image.h>
#include "Material.hpp"

class RenderTarget {
public:
  RenderTarget(int _w, int _h);
	void init();
	void begin(bool clear);

	void end();
	void deinit();

private:

  GLuint m_fbo;
  GLuint m_texture;
  GLuint m_vao;
  GLuint m_vbo;
  GLuint m_tonemap;
  Material* m_twotri;

	float gamma = .5f;
	float energy = 1.f;
   unsigned int HEIGHT = 100;
 unsigned int WIDTH = 100;

  int texWidth, texHeight, texChannels;
  stbi_uc* pixels = nullptr;

};

#endif //RENDER_TARGET_HPP