//
// Created by matt on 3/12/18.
//

#ifndef GL_PLAY_WINDOW_HPP
#define GL_PLAY_WINDOW_HPP
#include "Definitions.hpp"
#include <png.h>

class GLFWwindow;

class Window {

public:
  Window();
  int init(int width, int height);
  void resize(int width, int height);
  void screenshot_png(const char *filename, unsigned int width, unsigned int height,
      GLubyte **pixels, png_byte **png_bytes, png_byte ***png_rows);
public:
  GLFWwindow *window;
  int _width, _height;

  png_byte *png_bytes = NULL;
  png_byte **png_rows = NULL;

};

#endif // GL_PLAY_WINDOW_HPP
