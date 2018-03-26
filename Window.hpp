//
// Created by matt on 3/12/18.
//

#ifndef GL_PLAY_WINDOW_HPP
#define GL_PLAY_WINDOW_HPP

class GLFWwindow;

class Window {

public:
  Window();
  int init();

public:
  GLFWwindow *window;
};

#endif // GL_PLAY_WINDOW_HPP
