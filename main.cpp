#include "Definitions.h"
#include "Material.h"
#include "Processing.h"
#include "Triangulate.hpp"
#include <algorithm>

#include <iostream>
using namespace std;
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}
// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
Processing* ctx;
float t = 3;

void render(){
  std::vector<Tri> seed = { Tri{{0,.8},{-.8,-.8}, {.8, -.8}, 0} };
  ctx->polygon({{0,.8},{-.8,-.8}, {.8, -.8}}, true);
  Tri::triangulate(seed, t, ctx);

  ctx->flush();
}


void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
}

void drawCircle(int  numPts, float t, Processing* ctx );

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE){
    t++;
    //ctx->clear();
    drawCircle(t, t, ctx);

    ctx->flush();
    //render();
  }
}


int main() {

  glfwInit();
  cout<<glfwGetVersionString()<<endl;
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

  GLFWwindow *window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
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

  glViewport(0, 0, 800, 600);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetKeyCallback(window, keyCallback);

 // build and compile our shader program

  	Material basic{"basic.vert", "basic.frag"};
  ctx = new Processing{};
  render();
  //drawCircle(t,t,ctx);
    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    // glBindVertexArray(0);
  while (!glfwWindowShouldClose(window)) {
    processInput(window);
	  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	  glClear(GL_COLOR_BUFFER_BIT);
  
    basic.use();
    ctx->render();

    glfwSwapBuffers(window);
    glfwPollEvents();
  }


  glfwTerminate();
  return 0;
}

void render(float t) {
  float vertices[] = {-0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f, 0.0f, 0.5f, 0.0f};

}

void drawCircle(int  numPts, float t, Processing* ctx ){
 vector<vec2> pts{size_t(numPts)};
  float di = 6.28f / numPts;
  int i=0;
  for(auto& p : pts){
    p = vec2{sin(i*di)*.8f, cos(i*di + .2 * t)*.8f};
    i++;
  }
  ctx->polygon(pts,true);
}