#ifndef RENDER_TARGET_HPP
#define RENDER_TARGET_HPP
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
  Material* m_twotri;

   unsigned int HEIGHT = 100;
 unsigned int WIDTH = 100;
};

#endif //RENDER_TARGET_HPP