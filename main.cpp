#include "Definitions.hpp"
#include "Material.hpp"
#include "Processing.hpp"
#include "Triangulate.hpp"
#include <algorithm>
#include "Window.hpp"
#include "imgui.h"
#include "imgui_impl/glfw_gl3.h"
#include "ref/FastNoise/FastNoise.h"
#include "Blob.hpp"
#include "Plotter.hpp"
#include "RenderTarget.hpp"
#include "Flame.hpp"
#include "Streamline.hpp"

using namespace std;
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "Streamline.hpp"

#include "sketches.hpp"

// settings
Processing *ctx;
TriBuilder tri;
Plotter p;
float t = 0;
GLuint d;
int guessDepth;
static GLubyte *pixels = NULL;

void genTriangle() {
    if(tri.imSettings()){
    ctx->clear();
    tri.triangulate(t, ctx);
    ctx->flush();
  }
}

void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
}

void keyCallback(GLFWwindow *window, int key, int scancode, int action,
                 int mods) {
  return;
}

static void error_callback(int error, const char* description)
{
  fprintf(stderr, "Error %d: %s\n", error, description);
}


int main() {

  Window mainWin;
  mainWin.init(1000,1000);

  glfwSetErrorCallback(error_callback);
  glfwSetKeyCallback(mainWin.window, keyCallback);
  glEnable(GL_DEBUG_OUTPUT);
  // build and compile our shader program

  Material basic{"shaders/basic.vert", "shaders/basic.frag", true};
  Material flame{"shaders/basic.vert", "shaders/basic.frag", true, "shaders/flame.geom"};
  ctx = new ProcessingGL{};

  // Setup ImGui binding
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |=  ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
   io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad
  // Controls
  ImGui_ImplGlfwGL3_Init(mainWin.window, true);
  glPointSize(4);

  // Setup style
 // ImGui::StyleColorsDark();
  vec4 clear_color{0.05f, 0.15f, 0.16f, 1.00f};

  Blob b{{0,0}, .5f};
//  RenderTarget buff(2000,2000);
 // buff.init();
bool openDebug;
  vec4 params[4]{{.2,0,0,0},{.3,0,0,0},{.1,0,0,0},{.2,0,0,0}};
  int demoNumber = 3;
  bool trippy = false;
  while (!glfwWindowShouldClose(mainWin.window)) {
    glfwPollEvents();
   ImGui_ImplGlfwGL3_NewFrame();

    ImGui::ShowDemoWindow(&openDebug);
    // 1. Show a simple window.
    // Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets
    // automatically appears in a window called "Debug".
     ImGui::ColorEdit4(   "clear_color", (float *)&clear_color); // Edit 3 floats representing a color
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",  1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::InputInt("DemoNumber", &demoNumber,0,4);

    glClearColor(SPLAT4(clear_color));
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glViewport(0,0,mainWin._height, mainWin._height);

    bool redraw = false, clear = false;


    switch(demoNumber){
      case 0:
        genTriangle();
        basic.use();
        ((ProcessingGL*) ctx)->setMode(GL_TRIANGLES);
        ctx->render();
        break;
      case 1:
        do_curve(ctx, redraw, clear);
        if(redraw){
          ctx->render();
        }
        break;
      case 2:
        for(int i=0; i < 4; i++){
          ImGui::PushID(i);
          ImGui::ColorEdit4("arg", (float*)&(params[i]));
          ImGui::PopID();
        }

        ((ProcessingGL*) ctx)->setMode(GL_POINTS);
        Flame::do_flame(ctx, redraw, clear); // todo: maek it a return value, or put buffer inside of this function
        //buff.begin(clear);
        if(redraw) {
          flame.use();
          glUniform4fv(glGetUniformLocation(flame.ID, "u_adj"), 4, (float*) &params[0]);
          ctx->render();
        }
        glViewport(0,0,mainWin._height, mainWin._height);
// interlace a secondary buffer to save the final image?
       // buff.end();
        break;
      case 3:
        for(int i=0; i < 4; i++){
          ImGui::PushID(i);
          ImGui::ColorEdit4("arg", (float*)&(params[i]));
          ImGui::PopID();
        }
        if(ImGui::Checkbox("Trip out", &trippy)){
          flame.use();
          ((ProcessingGL*) ctx)->setMode(GL_POINTS);
        }
        else{
          basic.use();
          ((ProcessingGL*) ctx)->setMode(GL_TRIANGLES);
        }
        p.update(ctx, (float) glfwGetTime());
        break;
      default:
        break;
    }


    processInput(mainWin.window);

    t += .05f; // TODO: real deltatime

    ImGui::Render();
    ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(mainWin.window);

  } // end mainloop
  // Cleanup
  ImGui_ImplGlfwGL3_Shutdown();
  ImGui::DestroyContext();
  glfwTerminate();
  return 0;
}
