#include "Definitions.hpp"
#include "Material.hpp"
#include "Processing.hpp"
#include "Triangulate.hpp"
#include <algorithm>
#include "glm/ext.hpp"
#include "Random.hpp"
#include "Window.hpp"
#include "imgui.h"
#include "imgui_impl/glfw_gl3.h"
#include "ref/FastNoise/FastNoise.h"
#include "Blob.hpp"

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

  const float pi = 3.1415f;
  const float tau = pi * 2;

  static vec2 frequency{5, .1f};
  static float magnitude = 2;
  static int samples = 64;
  static int flowerSeed = 0;
  static float fullscale = 1;
  static float decay = .1f;
  static vec4 startColor{1, 0, 0, 0};
  static vec4 endColor{1, 1, 0, 1};

  bool redraw = false;
  redraw |= ImGui::InputInt("Seed", &flowerSeed);
  redraw |= ImGui::SliderFloat2("Frequency", (float *)&frequency, 1.f, 400.f);
  redraw |= ImGui::SliderFloat("Magnitude", &magnitude, 0.f, 10.f);
  redraw |= ImGui::SliderInt("Samples", &samples, 4, 256);
  redraw |= ImGui::SliderFloat("Decay", &decay, .01f, 1.f);
  redraw |= ImGui::SliderFloat("Fullscale", &fullscale, .001f, 2);
  if( ! redraw) return;
  ctx->clear();
  FastNoise noise;
  noise.SetSeed(flowerSeed);

  auto noisePoint = [=](float i, float theta) {
    vec3 pos{sin(theta), cos(theta),0};
    float r = i + decay * magnitude * (.5 - noise.GetNoise(pos.x * frequency[0] , pos.y * frequency[0], i * frequency[1]));
    return pos * r * fullscale;
  };
  auto noiseColor = [=](float i, float theta) {
    return mix(startColor, endColor, abs(sin(theta/2)+i));
  };

  float dTheta = 2 * pi / samples;
  UI_Vertex center{{0, 0, 0}, {0,0,0,0}};
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


void doPlacement(Processing * ctx){

  static bool redraw = false;
  static float rmin = .05;
  static float rmax = .2;
  static int samples = 100;
  static float overlap = .04f;
  static int seed = 0;
  redraw |= ImGui::InputInt("Seed", &seed);
  redraw |= ImGui::SliderFloat("rMin", &rmin, 0.001f, rmax);
  redraw |= ImGui::SliderFloat("rMax", &rmax, rmin, 1.f);
  redraw |= ImGui::SliderFloat("overlap", &overlap, 0.001f, 1.f);
  redraw |= ImGui::SliderInt("samples", &samples, 20, 5000);
  if(!redraw){
    return;
  }

  Random::seed(seed);
  vector<Blob*> blobs;
  Partition p(2,2, {.5, .5});
  p.gen_poisson({-1,-1}, {1,1}, rmin, rmax, samples, blobs, overlap);

  ctx->clear();
  for(auto b : blobs){
    b->render(ctx);
    delete(b);
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

  Blob b{{0,0}, .5f};
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

    doPlacement(ctx);
    
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
