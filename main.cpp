#include "Definitions.hpp"
#include "Material.hpp"
#include "Processing.hpp"
#include "Triangulate.hpp"
#include <algorithm>

#include "Random.hpp"
#include "Window.hpp"
#include "imgui.h"
#include "imgui_impl/glfw_gl3.h"

using namespace std;
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}
// settings
Processing *ctx;
TriBuilder tri;
float t = 3;

void genTriangle() {
  std::vector<Tri> seed = {Tri{{0, .8, 0}, {-.8, -.8, 0}, {.8, -.8, 0}, 0}};

  tri.triangulate(seed, ctx);
  ctx->flush();
}

void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
}

void drawCircle(int numPts, float t, Processing *ctx);

void keyCallback(GLFWwindow *window, int key, int scancode, int action,
                 int mods) {
  return;
}

void spawnFlower(Processing *ctx) {
  static float frequency = 5;
  static float magnitude = 2;
  static float samples = 64;
  static float decay = .1f;
  static vec4 startColor{1, 0, 0, 0};
  static vec4 endColor{0, 1, 0, 1};

  const float pi = 3.1415f;
  const float tau = pi * 2;

  auto noisePoint = [=](float i, float theta) {
    return vec3{sin(theta) * i, cos(theta) * i, 0};
  };
  auto noiseColor = [=](float i, float theta) {
    return mix(startColor, endColor, abs(sin(theta)));
  };
  float dTheta = 2 * pi / samples;
  UI_Vertex center{{0, 0, 0}};
  for (float i = 1; i > .001f; i -= decay) {
    for (float theta = 0; theta < (2 * pi); theta += dTheta) {
      auto a = noisePoint(i, theta);
      auto b = noisePoint(i, theta + dTheta);
      ctx->tri(UI_Vertex{a, noiseColor(i, theta)},
               UI_Vertex{b, noiseColor(i, theta + dTheta)}, center);
    }
  }
  ctx->flush();
}

int main() {

  Window mainWin;
  mainWin.init();

  glfwSetFramebufferSizeCallback(mainWin.window, framebuffer_size_callback);
  glfwSetKeyCallback(mainWin.window, keyCallback);

  // build and compile our shader program

  Material basic{"basic.vert", "basic.frag", true};
  ctx = new Processing{};
  genTriangle();

  // Setup ImGui binding
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
  // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad
  // Controls
  ImGui_ImplGlfwGL3_Init(mainWin.window, true);

  // Setup style
  ImGui::StyleColorsDark();
  vec4 clear_color{0.45f, 0.55f, 0.60f, 1.00f};

  static int counter = 0;

  while (!glfwWindowShouldClose(mainWin.window)) {
    glfwPollEvents();
    ImGui_ImplGlfwGL3_NewFrame();

    bool redraw = false;
    // 1. Show a simple window.
    // Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets
    // automatically appears in a window called "Debug".
    ImGui::ColorEdit3(
        "clear color",
        (float *)&clear_color); // Edit 3 floats representing a color
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    redraw = tri.imSettings();
    if (redraw) {
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
