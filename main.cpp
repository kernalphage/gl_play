#include "Definitions.h"
#include "Material.h"
#include "Processing.h"
#include "Triangulate.hpp"
#include <algorithm>


#include "imgui.h"
#include "imgui_impl/glfw_gl3.h"
#include "Window.hpp"
#include "Random.hpp"

using namespace std;
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}
// settings
Processing* ctx;
TriBuilder tri;
float t = 3;

void genTriangle(){
  std::vector<Tri> seed = { Tri{{0,.8,0},{-.8,-.8,0}, {.8, -.8,0}, 0} };
  ctx->polygon({{0,.8,0},{-.8,-.8,0}, {.8, -.8,0}}, true);
  tri.triangulate(seed, ctx);
  ctx->flush();
}


void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
}

void drawCircle(int  numPts, float t, Processing* ctx );

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  return;
}


int main() {

  Window mainWin;
  mainWin.init();

  glfwSetFramebufferSizeCallback(mainWin.window, framebuffer_size_callback);
  glfwSetKeyCallback(mainWin.window, keyCallback);

 // build and compile our shader program

	Material basic{"basic.vert", "basic.frag"};
  ctx = new Processing{};
  Random::seed();
    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    // glBindVertexArray(0);


    // Setup ImGui binding
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls
    ImGui_ImplGlfwGL3_Init(mainWin.window, true);

    // Setup style
    ImGui::StyleColorsDark();
    vec4 clear_color {0.45f, 0.55f, 0.60f, 1.00f};
    
  static int counter = 0;

  while (!glfwWindowShouldClose(mainWin.window)) {
    glfwPollEvents();
    ImGui_ImplGlfwGL3_NewFrame();
    bool redraw = false;
        // 1. Show a simple window.
        // Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets automatically appears in a window called "Debug".
        {
            redraw = tri.imSettings();
            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        }
        if(redraw){
          ctx->clear();
          genTriangle();
        }
        processInput(mainWin.window);

        glClearColor(SPLAT4(clear_color));
        glClear(GL_COLOR_BUFFER_BIT);
      
        basic.use();
        ctx->render();

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

void drawCircle(int  numPts, float t, Processing* ctx ){
 vector<vec3> pts{size_t(numPts)};
  float di = 6.28f / numPts;
  int i=0;
  for(auto& p : pts){
    p = vec3{sin(i*di)*.8f, cos(i*di + .2 * t)*.8f,0};
    i++;
  }
  ctx->polygon(pts,true);
}