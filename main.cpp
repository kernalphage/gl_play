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
int guessDepth;
static GLubyte *pixels = NULL;

#define impl_STATIC_DO_ONCE(x, y) {static bool __doonce##y=true; if(__doonce##y == true){__doonce##y = false; x;}};
#define STATIC_DO_ONCE(x) impl_STATIC_DO_ONCE(x, __LINE__);

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
  int maxIterations = 9; // todo: better heuristic, this will be ~ sizeof(init) * 512

	do {
		vector<T> output{ seed[0] };
		bool needed_cut = false;
		for(int i=0; i < seed.size() - 1; i++){
			if(distance(seed[i], seed[i+1]) < minDist) { //i think it's fucky here?
        output.push_back( seed[i] );
        continue;
		 	};
			needed_cut = true;
			T q = mix(seed[i], seed[i+1], smooth);
			T r = mix(seed[i], seed[i+1], 1 - smooth);
			output.push_back( q );
			output.push_back( r );
		}
    if(!needed_cut){
      return output;
    }
    seed = output;
	} while(maxIterations-- > 0);
  return seed;
}

void do_curve(Processing* ctx){
  static int seed = 0;
  static int numPts = 5;
  static float minDist = 1;
  static float spikiness = 0.f;
  static float thickness = 0.f;
  bool redraw = false;
  redraw |= ImGui::InputInt("Seed", &seed);
  redraw |= ImGui::SliderInt("numPts", &numPts, 2, 50);
  redraw |= ImGui::SliderFloat("minDist", &minDist, 0.001f, 2.f);
  redraw |= ImGui::SliderFloat("spikiness", &spikiness, 0.f, 1.f);
  redraw |= ImGui::SliderFloat("thickness", &thickness, 0.f, 1.f);

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

  ctx->spline(pts, {1.,.5,.7,1}, {.5, .5,.7, 0}, thickness);
  ctx->flush();
}
void do_flame(Processing * ctx, bool& _r, bool& _c){
  // TODO: abstract the heatmap from the thing that draws on the heatmap

  static int seed = 12;
  static int numPts = 12;
  static int numLayers = 2;
  static int num_iterations = 8;
  static int curLayer = 0;
  static float startScale = 3;
  static float endScale = 1.f;
  static vector<Flame> ff(5);

  static vec4 color{1.f, 1.f, 0.f, 1};

  bool redraw = false;
  bool reseed = false;

  STATIC_DO_ONCE(redraw = true;);
  STATIC_DO_ONCE(reseed = true;);
  // Typically we would use ImVec2(-1.0f,0.0f) to use all available width, or ImVec2(width,0.0f) for a specified width. ImVec2(0.0f,0.0f) uses ItemWidth.
  ImGui::ProgressBar((float) curLayer / numLayers, ImVec2(0.0f,0.0f));
  ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
  ImGui::Text("Progress Bar");

  redraw |= reseed = ImGui::InputInt("Seed", &seed);
  ImGui::SliderInt("Layers", &numLayers, 2, 100);
  redraw |= ImGui::SliderInt("numPts", &numPts, 2, 500);
  redraw |= ImGui::SliderInt("NumIterations", &num_iterations, 2,99);
  redraw |= ImGui::SliderFloat("startScale", &startScale, .05, 5);
  redraw |= ImGui::SliderFloat("endScale", &endScale, .05, 5);

  for(int i=0; i < ff.size(); i++){
    if (ImGui::TreeNode((void*)(intptr_t)i, "Flame %d", i)) {
      redraw |= ff[i].imSettings(i);
      ImGui::TreePop();
    }
  }

  if(!redraw && curLayer >= numLayers )
  {
    redraw = false;
    _r = false;
    _c = false;
    return;
  }
  cout<<"Layer "<< curLayer<<endl;
  curLayer++;
  ctx->clear();
  if(redraw || reseed){
    curLayer = 0;
  }
  if(reseed){
    for(auto&f:ff){ f.randomInit();};

  }
  Random::seed(seed + curLayer);

  float di = 2.0f / numPts;
  for(int i=0; i < numPts; i++){
    for(int j = 0; j < numPts; j++){
      float pi= 1.f*i/numPts , pj= 1.f*j/numPts;
      vec2 p = Random::random_point({-startScale, -startScale}, {startScale, startScale});
      vec4 xcolor = vec4(abs(p.x),abs(p.y), 0, 1 );
      vec4 aColor = vec4(xcolor.x, xcolor.y, xcolor.z, 0);

      for(int i =0; i < num_iterations; i++) {
        auto f = ff[Random::range(0, ff.size())];
        p = f.fn(p);
      }
      p *= endScale;

      vec3 pp{p.x,p.y, 0};
      float r = .02;
      float thickness = r;
      const float tau = 6.282;
      vec3 threepos{p, 1};
      float dTheta = (tau) / 4;

      auto cPos = [=](float t, float radius){ return vec3{sin(t), cos(t), 1} * radius + threepos;};
      for (float theta = dTheta; theta <= (tau); theta += dTheta) {
        ctx->quad(UI_Vertex{cPos(theta, r), aColor},
                  UI_Vertex{cPos(theta + dTheta, r), aColor},
                  UI_Vertex{cPos(theta + dTheta, r  - thickness), xcolor},
                  UI_Vertex{cPos(theta, r - thickness), xcolor});
      }

      /*
      ctx->quad(UI_Vertex{pp+vec3{.1,0,0}, color},
                    UI_Vertex{pp+vec3{.1,.1,0}, color},
                    UI_Vertex{pp+vec3{0,.1,0}, color},
                    UI_Vertex{pp+vec3{0,0,0}, color});
                    */
    }
  }

  ctx->flush();
  _r = true;
  _c = curLayer == 0;
  return ;
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

static void error_callback(int error, const char* description)
{
  fprintf(stderr, "Error %d: %s\n", error, description);
}


void test_chaikin(){
  vector<vector<float>> params{
      {0, 16},
      {0,1,16},
      {0,1,16, 16},
      {0,0,1,4,7,9,11,3,444,16,16},
  };

  for(auto line : params){
    auto chain = chaikin(line, .25, 1);
    for(auto c : chain){
      cout<< c<<" ";
    }
    cout<<endl;
  }
}


int main() {

  Window mainWin;
  mainWin.init(900,500);

  glfwSetErrorCallback(error_callback);
  glfwSetKeyCallback(mainWin.window, keyCallback);
  glEnable(GL_DEBUG_OUTPUT);
  // build and compile our shader program

  Material basic{"basic.vert", "basic.frag", true};
  ctx = new ProcessingGL{};

  // Setup ImGui binding
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |=  ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
   io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad
  // Controls
  ImGui_ImplGlfwGL3_Init(mainWin.window, true);

  // Setup style
 // ImGui::StyleColorsDark();
  vec4 clear_color{0.05f, 0.15f, 0.16f, 1.00f};

  Blob b{{0,0}, .5f};
  RenderTarget buff(500,500);
  buff.init();
bool openDebug;
  while (!glfwWindowShouldClose(mainWin.window)) {
    glfwPollEvents();
   ImGui_ImplGlfwGL3_NewFrame();

    ImGui::ShowDemoWindow(&openDebug);
    // 1. Show a simple window.
    // Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets
    // automatically appears in a window called "Debug".
     ImGui::ColorEdit4(   "clear_color", (float *)&clear_color); // Edit 3 floats representing a color
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",  1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    glClearColor(SPLAT4(clear_color));
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    bool redraw = false, clear = false;

    //genTriangle();
  //  do_curve(ctx);
    do_flame(ctx, redraw, clear); // todo: maek it a return value, or put buffer inside of this function
  //p.update(ctx, (float) glfwGetTime());
    processInput(mainWin.window);

   buff.begin(clear);
   if(redraw) {
     basic.use();
     ctx->render();
   }
    buff.end();

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
