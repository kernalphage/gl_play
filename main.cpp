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

using namespace std;
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

// settings
Processing *ctx;
TriBuilder tri;
Plotter p;
float t = 0;
GLuint d;
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


template<typename T>
vector<T> chaikin(vector<T> init, float smooth, float minDist){
  // http://graphics.cs.ucdavis.edu/education/CAGDNotes/Chaikins-Algorithm/Chaikins-Algorithm.html
  vector<T> seed = init;
  int maxIterations = seed.size() * 10; // todo: better heuristic ;

	do {
		vector<T> output{ seed[0] };
		bool needed_cut = false;
		for(int i=0; i < seed.size() - 1; i++){
			if(distance(seed[i], seed[i+1]) < minDist) { //i think it's fucky here?
        output.push_back( seed[i] );
        output.push_back( seed[i+1] );

        continue;
		 	};
			needed_cut = true;
			T q = mix(seed[i], seed[i+1], smooth);
			T r = mix(seed[i], seed[i+1], 1 - smooth);
			output.push_back( q );
			output.push_back( r );
		}
		seed = output;
	} while(maxIterations-- > 0);
}

void do_curve(Processing* ctx){
  static int seed = 0;
  static int numPts = 5;
  static float minDist = 1;
  static float spikiness = 0.f;
  bool redraw = false;
  redraw |= ImGui::InputInt("Seed", &seed);
  redraw |= ImGui::SliderInt("numPts", &numPts, 2, 50);
  redraw |= ImGui::SliderFloat("minDist", &minDist, 0.001f, 2.f);
  redraw |= ImGui::SliderFloat("spikiness", &spikiness, 0.f, 1.f);

  if(!redraw) return;
  ctx->clear();
  Random::seed(seed);
  vector<vec3> pts;
  for(int i=0; i < numPts; i++){
    vec3 newpt{ Random::range(-1.f,1.f),Random::range(-1.f,1.f),Random::range(-1.f,1.f)};
    if( Random::f() < spikiness && i > 1){
      pts.push_back( pts[ i ]);
      continue;
    }
    pts.push_back(newpt);
  }
  pts = chaikin(pts,.3333, minDist);

  ctx->spline(pts, {.2,.5,.7,.1});
  ctx->flush();
}
void do_flame(Processing * ctx){
  static int seed = 0;
  static int numPts = 5;
  static vec2 offset{0,0};
  static vec4 vars{0,0,0,0};
  static float strength = .5;
  static float domain = 3;

  static vec4 color{1, 0, 0, 1};
  bool redraw = false;

  redraw |= ImGui::InputInt("Seed", &seed);
  redraw |= ImGui::SliderInt("numPts", &numPts, 2, 500);
  redraw |= ImGui::SliderFloat2("offset", (float*)&offset, -2,2);
  redraw |= ImGui::SliderFloat4("vars", (float*)&vars, -2,2);
  redraw |= ImGui::SliderFloat("strength", &strength, 0, 1);
  redraw |= ImGui::SliderFloat("domain", &domain, .5, 5);


  if(!redraw) return;
  ctx->clear();
  Random::seed(seed);
  float di = 2.0f / numPts;
  for(int i=0; i < numPts; i++){
    for(int j = 0; j < numPts; j++){
      vec2 p = Random::random_point({-domain,-domain}, {domain,domain} ) ;
      p = mix( p, Flame::swirl(p + offset, vars), strength);

      p /= domain;
      vec3 pp{p.x,p.y, 0};

      ctx->quad(UI_Vertex{pp+vec3{.01,0,0}, color},
                    UI_Vertex{pp+vec3{.01,.01,0}, color},
                    UI_Vertex{pp+vec3{0,.01,0}, color},
                    UI_Vertex{pp+vec3{0,0,0}, color});
    }
  }

  ctx->flush();
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

int main() {

  Window mainWin;
  mainWin.init(500,500);

  glfwSetKeyCallback(mainWin.window, keyCallback);

  // build and compile our shader program

  Material basic{"basic.vert", "basic.frag", true};
  ctx = new ProcessingGL{};

  // Setup ImGui binding
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
  // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad
  // Controls
  ImGui_ImplGlfwGL3_Init(mainWin.window, false);

  // Setup style
  ImGui::StyleColorsDark();
  vec4 clear_color{0.95f, 0.95f, 0.96f, 1.00f};

  Blob b{{0,0}, .5f};
 // RenderTarget buff(500,500);

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
  //  do_curve(ctx);
    do_flame(ctx);
  //p.update(ctx, (float) glfwGetTime());
    processInput(mainWin.window);

    glClearColor(SPLAT4(clear_color));
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //buff.activate();
    basic.use();
    ctx->render();


    //buff.render();
    //basic.use();
    //ctx->render();
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
