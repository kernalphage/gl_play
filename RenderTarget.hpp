#ifndef RENDER_TARGET_HPP
#define RENDER_TARGET_HPP


class RenderTarget {

	void init();


	void deinit();

private:

 GLubyte *pixels = NULL;
 GLuint fbo;
 GLuint rbo_color;
 GLuint rbo_depth;
 const unsigned int HEIGHT = 100;
 const unsigned int WIDTH = 100;
 int offscreen = 1;
 unsigned int max_nframes = 100;
 unsigned int nframes = 0;
 unsigned int time0;
};

#endif //RENDER_TARGET_HPP