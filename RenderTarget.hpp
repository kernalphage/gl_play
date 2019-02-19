#ifndef RENDER_TARGET_HPP
#define RENDER_TARGET_HPP

#include <stb/stb_image.h>
#include "Material.hpp"
#include "Texture.hpp"

class RenderTarget {
public:
  RenderTarget(int _w, int _h);
	void init();
	void begin(bool clear);

	void end();
	void deinit();
	void save(const char*const filename);
private:

  GLuint m_fbo;
  GLuint m_texture;
  GLuint m_vao;
  GLuint m_vbo;
  Material* m_twotri;

	float gamma = .1f;
	float texAmt = 0.0f;
	float energy = 2.f;
   unsigned int HEIGHT = 100;
 unsigned int WIDTH = 100;

   Texture m_tonemap;

};

#endif //RENDER_TARGET_HPP