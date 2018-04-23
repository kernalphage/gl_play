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
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

using namespace std;

// settings
Processing *ctx;
TriBuilder tri;
int texWidth, texHeight, texChannels;
stbi_uc* pixels;

float t = 0;

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

void spawnFlower(Processing *ctx) {

  const float pi = 3.1415f;

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

vec3 sampleImage(const vec2 pos, int channel = 0) {
      vec2 v2 = (pos + vec2{1, 1}) / 2;
      int ex = (int) floor((v2.x) * texWidth);
      int wy = (int) floor((v2.y) * texHeight);
      ex = std::min(std::max(ex, 0), texWidth - 1);
      wy = std::min(std::max(texHeight - wy, 0), texHeight - 1);
      int idx = (ex + wy * texWidth) * 4;
      return vec3{pixels[idx + 0],pixels[idx + 1],pixels[idx + 2]} / 255;
};

struct LayerSettings
{
  vector<Blob> blobs;
  vec4 color;
  bool ImSettings(int id){
    ImGui::PushID(id);
    bool recolor = false;
    recolor |= ImGui::ColorEdit4("Color", (float *)&color);
    ImGui::PopID();
    return recolor;
  }
};

string timestamp(){
  char mbstr[100];
   std::time_t t = std::time(nullptr);
  std::strftime(mbstr, sizeof(mbstr), "%m_%d_%T.svg", std::localtime(&t));
  cout<<mbstr<<endl;
  return string(mbstr);
}

void doPlacement(Processing * ctx){

  static bool redraw = false;
  static bool recolor = false;
  static float rmin = .01;
  static float rmax = .15;
  static float showThreshhold = .90;
  static int samples = 1400;
  static float overlap = 4.84f;
  static vec4 innerColor;
  static int seed = 0;
  static int numLayers = 3;
  static vector<LayerSettings> layers = {{{},{1.0,0.0,0.0, 1.0}}, {{},{0.0,1.0,0.0, 1.0}}, {{},{0.0,0.0,1.0, 1.0}}};
  static float gamma = 2;
  static float thickness = .01;
  const char* listbox_items[] = { "random", "circular", "sinwave", "image", "wipe"};
  static int listbox_item_current = 1;
  static bool useImageColors = false;
    
  redraw = ImGui::Button("redraw");
  ImGui::SameLine();
  redraw |= ImGui::InputInt("Seed", &seed);
  redraw |= ImGui::ListBox("Render Mode\n(single select)", &listbox_item_current, listbox_items, IM_ARRAYSIZE(listbox_items), 5);

  redraw |= ImGui::InputInt("NumLayers", &numLayers, 1, 10);
  layers.resize(numLayers);

  ImGui::SliderFloat("rMin", &rmin, 0.0001f, rmax/2);
  ImGui::SliderFloat("rMax", &rmax, rmin, 1.f);
  ImGui::SliderInt("samples", &samples, 20, 5000);
  ImGui::SliderFloat("frequency", &overlap, 0, 30);
  ImGui::SliderFloat("SizeGamma", &gamma, 0, 20);

  recolor = false;
  recolor |= ImGui::SliderFloat("thickness", &thickness, 0, .05f);
  recolor |= ImGui::SliderFloat("showThreshhold", &showThreshhold, 0, 1.5);
  recolor |= ImGui::Checkbox("Use Image Colors", &useImageColors);
  for(int i=0; i < layers.size(); i++) {
      auto layer = layers[i];
      recolor |= layers[i].ImSettings(i);
  }

  if(redraw){
    int jiggle = 0;
    for(int i=0; i < layers.size(); i++) {
      auto& layer = layers[i];

      auto radiusFN = [=](const vec2 pos) {
        float metric;
        switch(listbox_item_current){
          case 0:
            metric = Random::range(0.0f, 1.0f);
            break;
          case 1:
            metric = length(pos);
            break;
          case 2:
            metric = abs(cos(pos.x * overlap) + sin(pos.y * overlap))/2;
            break;
          case 3:
            metric = distance((vec3{SPLAT3(layer.color)}), (sampleImage(pos)));
            break;
          case 4:
            metric = abs(dot(pos, vec2{1,0}));
            break;
        }
        metric = std::min(std::max(metric, 0.01f), 0.99f);
        //metric = pow(metric, gamma);
        float sz = mix(rmin, rmax, metric);
        return vec2{sz * .5, sz};
      };
      Random::seed(seed + i*10 + jiggle);
      vector<Blob>& blobs = layer.blobs;
      blobs.clear();
      Partition p({-.8, -.8}, {1.6f, 1.6f}, rmax * 2);

      cout<<"function " << listbox_item_current<< endl;


      p.gen_poisson({-.8, -.8}, {.8, .8}, radiusFN, samples, blobs, 0.0001f);
      cout << "Blobs " << blobs.size() << " generated";
      if(blobs.size() == 1 && jiggle < 5){
        i--;
        jiggle ++;
      }
    }
  }

  if(redraw || recolor){   
    ctx->clear();
     for(int i=0; i < numLayers; i++) {
      for(auto b: layers[i].blobs){
        if(b.r <= rmax * showThreshhold){
          if(useImageColors){
            b.render(ctx, vec4{sampleImage(b.pos),1.0},thickness);
          }
          else{
            b.render(ctx, layers[i].color, thickness);
          }
        }
      }
     }
    ctx->flush();
  }

  if( ImGui::Button("Save")){
    ProcessingSVG svg;
     for(int i=0; i < numLayers; i++) {
      svg.setFilename(timestamp());
      for(const auto& b: layers[i].blobs){
        if(b.r <= rmax * showThreshhold){
          svg.circle(b.pos, b.r);
        }
      }
      svg.render();
     }

  }

}

int main() {

  Window mainWin;
  mainWin.init(500,500);

  glfwSetKeyCallback(mainWin.window, keyCallback);

  // build and compile our shader program

  Material basic{"basic.vert", "basic.frag", true};
  ctx = new ProcessingGL{};
  pixels = stbi_load("oni.jpg", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

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
  vec4 clear_color{0.045f, 0.05f, 0.06f, 1.00f};

  Blob b{{0,0}, .5f};
  while (!glfwWindowShouldClose(mainWin.window)) {
    glfwPollEvents();
    ImGui_ImplGlfwGL3_NewFrame();

    // 1. Show a simple window.
    // Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets
    // automatically appears in a window called "Debug".
    ImGui::ColorEdit3(
        "clear_color",
        (float *)&clear_color); // Edit 3 floats representing a color
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

  //genTriangle();
  doPlacement(ctx);
    t += .05f; // TODO: real deltatime
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
