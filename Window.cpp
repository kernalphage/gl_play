//
// Created by matt on 3/12/18.
//

#include "Window.hpp"
#include "Definitions.hpp"
#include <iostream>
#include <png.h>
using namespace std;

Window::Window() {}
int Window::init(int width, int height) {
  _width = width;
  _height = height;

  glfwInit();
  cout << glfwGetVersionString() << endl;
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
  glfwWindowHint(
      GLFW_OPENGL_FORWARD_COMPAT,
      GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

  window = glfwCreateWindow(_width,_height, "LearnOpenGL", NULL, NULL);
  if (window == NULL) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }


  glfwSetWindowUserPointer(window, this);
  // Ugly? Maybe.
  glfwSetFramebufferSizeCallback(window,
              [](GLFWwindow *window, int w, int h){
                auto& self = *static_cast<Window*>(glfwGetWindowUserPointer(window));
                  self.resize(w, h);});

  glViewport(0, 0, _width, _height);
  return 0;
}

void Window::resize(int width, int height) {
  _width = width;
  _height = height;
  glViewport(0,0,_width, _height);

}

/*
 //Adapted from https://github.com/cirosantilli/cpp-cheat/blob/19044698f91fefa9cb75328c44f7a487d336b541/png/open_manipulate_write.c
 void Window::screenshot_png(const char *filename, unsigned int width, unsigned int height,
                           GLubyte **pixels, png_byte **png_bytes, png_byte ***png_rows) {
  size_t i, nvals;
  const size_t format_nchannels = 4;
  FILE *f = fopen(filename, "wb");
  nvals = format_nchannels * width * height;
  *pixels = (GLubyte*) realloc(*pixels, nvals * sizeof(GLubyte));
  *png_bytes = (png_byte*) realloc(*png_bytes, nvals * sizeof(png_byte));
  *png_rows = (png_byte **) realloc(*png_rows, height * sizeof(png_byte*));
  glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, *pixels);
  for (i = 0; i < nvals; i++)
    (*png_bytes)[i] = (*pixels)[i];
  for (i = 0; i < height; i++)
    (*png_rows)[height - i - 1] = &(*png_bytes)[i * width * format_nchannels];
  png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!png) abort();
  png_infop info = png_create_info_struct(png);
  if (!info) abort();
  if (setjmp(png_jmpbuf(png))) abort();
  png_init_io(png, f);
  png_set_IHDR(
      png,
      info,
      width,
      height,
      8,
      PNG_COLOR_TYPE_RGBA,
      PNG_INTERLACE_NONE,
      PNG_COMPRESSION_TYPE_DEFAULT,
      PNG_FILTER_TYPE_DEFAULT
  );
  png_write_info(png, info);
  png_write_image(png, *png_rows);
  png_write_end(png, NULL);
  fclose(f);
}
 */