#include "Definitions.hpp"
#include "Material.hpp"
#include "Processing.hpp"
#include "Triangulate.hpp"
#include "Window.hpp"
#include "imgui.h"
#include "imgui_impl/glfw_gl3.h"
#include "Blob.hpp"
#include "Plotter.hpp"
#include "RenderTarget.hpp"
#include "Flame.hpp"
#include "Streamline.hpp"
#include "flowmap.hpp"
#include <rapidjson/rapidjson.h>
#include "RandomCache.hpp"


using namespace std;
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "Streamline.hpp"

#include "sketches.hpp"
#include "proc_map.hpp"

// settings
Processing *ctx;
TriBuilder tri;
Plotter p;
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

static void error_callback(int error, const char* description)
{
  fprintf(stderr, "Error %d: %s\n", error, description);
}

void drawNoise(Processing* ctx, bool& redraw, bool& clear){
  STATIC_DO_ONCE(clear = true);
  static FastNoise n;
  static RandomCache rx{500, 2, [](vec2 pos){return n.GetValueFractal(pos.x, pos.y, 0);}};
  static RandomCache ry{500, 2, [](vec2 pos){return n.GetValueFractal(pos.x, pos.y, 100);}};
  static float noisefreq = 2.31511;
  static int numpts = 300;
  static float maxVel = .005;
  static vector<vec4> pts;
  static float angleVel = .1;
  static int numIterations = 1;
  static float totalTime = 0;

  bool reNoise = ImGui::SliderFloat("frequency", &noisefreq, .01, 15);
  reNoise = true;
  if(reNoise){
    totalTime += .01;
    n.SetFrequency(noisefreq);
    rx.reseed(1, [](vec2 pos){return n.GetValueFractal(pos.x, pos.y + sin(totalTime), cos(totalTime));});
    ry.reseed(1, [](vec2 pos){return n.GetValueFractal(pos.x, pos.y + sin(totalTime),100+ cos(totalTime));});
    clear = true;
  }

  clear |= ImGui::SliderInt("numPts", &numpts, 10, 5000);
  clear |= ImGui::SliderFloat("velocity", &maxVel, 0,.1);
  clear |= ImGui::SliderFloat("angleVel", &angleVel, 0,20);
  ImGui::SliderInt("num iterations", &numIterations, 1, 100);

  if(clear){
    pts.clear();

    // init points
    for(int i=0; i < numpts; i++) {
      float ex = Random::nextGaussian() * .5f;
      float wy = Random::nextGaussian() * .5f;
      auto pt = Random::random_point({-1, -1, -1, -1}, {1,1, 1, 1});
      pt.x = ex;
      pt.y = wy;
      pts.push_back(pt);
    }
  }
  redraw = true;
  for(int iter = 0; iter < numIterations; iter++) {
    for (int i = 0; i < numpts; i++) {

      /*
      auto pt = vec2(pts[i].x, pts[i].y);
      auto vel = vec2(pts[i].z, pts[i].w);
      float noisePt = r.sample((pt * .5) + vec2(.5, .5));

      pts[i].x += cos(vel.x) * maxVel*.1;
      pts[i].y += sin(vel.x) * maxVel*.1;
      pts[i].z = (noisePt)* angleVel * glm::length(pt);
      */

      float ex = Random::nextGaussian() * .5f;
      float wy = Random::nextGaussian() * .5f;
      float radius = angleVel * sqrt(ex*ex +wy*wy);
      vec2 pos {ex,wy};
      pos = pos * .5 + vec2{.5,.5};
      pos = radius * vec2{rx.sample(pos), ry.sample(pos)};
      vec4 color = lerp(vec4{.5,.9, .1, 1}, vec4{.2,.6,.9,1}, abs(ex));
      ctx->ngon(pos, maxVel, 6, vec4{SPLAT3(color), 0}, color);
    }
  }
}

void drawPendulum(Processing* ctx, bool& redraw, bool& clear){
  redraw = true;
  clear = false;
  STATIC_DO_ONCE(clear = true;);

  static float time = 0;
  static float deltaTime = .001;
  static int numIterations = 2;

  static float dAngle = .0001;
  static float ratio = 1;
  static float radius_ratio = 1;
  static int cutoff = 0;
  ImGui::SliderInt("num interations", &numIterations, 1, 1000);
  clear |= ImGui::SliderInt("cutoff", &cutoff, 0, 10);
  clear |= ImGui::DragFloat("angleSpeed", &dAngle, .001, 0.01);
  clear |= ImGui::DragFloat("ratio", &ratio, .001, 21);
  clear |= ImGui::DragFloat("radius_ratio", &radius_ratio, .001, 1);
  clear |= ImGui::DragFloat("DeltaTime", &deltaTime, 0.001, 1);

  static float curAngle = 0;
  for(int iter = 0; iter < numIterations; iter++) {
    curAngle += dAngle * .1;
    time += deltaTime;
    vec3 cur{0,0,0};
    vector<vec3> points{};
    for (int i = 0; i < 10; i++) {
      //float nextAngle = curAngle / (i + 1);
      float nextAngle = curAngle  * pow(ratio, i);
      if(i%2 ==0) nextAngle *= -1;
      vec3 next{sin(nextAngle), cos(nextAngle),0};
      next = (next * ((10 - i) * radius_ratio)/10) + cur;
      if(i > cutoff) {
        points.push_back(next);
        //ctx->ngon(next, .004,8, vec4{1,1,1,0}, vec4{1,1,1,1} );
      }
      cur = next;
//      ctx->line(cur, next);
    }
    float r = abs(cos(time));
    float g = 1-r;
    ctx->spline(points, vec4{r,.1, .5, 1}, vec4{r,.1,.5, .0}, .001);
  }
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
  Material particle{"shaders/particle.vert", "shaders/textured.frag", true, "shaders/particle.geom"};
  Texture m_tonemap;
  m_tonemap.load("particle.png");
  particle.setInt("textured.frag", 0);

  ctx = new ProcessingGL_t<UI_Vertex, vec4>{};
  auto* part_ctx = new ProcessingGL_t<Particle_Vertex, Particle_Vertex::particle_data>{};

  // Setup ImGui binding
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |=  ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad
  // Controls
  ImGui_ImplGlfwGL3_Init(mainWin.window, true);

  vec4 clear_color{0.00f, 0.0f, 0.0f, 1.00f};


  flowmap f;
  proc_map map;

  Streamline s(1,1);
  //s.stream_point({0.5f,0.5f});
  for(int i=0; i < 100; i++){
    vec2 p = Random::random_point({0,0}, {1,1});
    s.stream_point(p);
  }

  Blob b{{0,0}, .5f};
  RenderTarget buff(2000,2000);
  buff.init();
  vec4 params[4]{{.2,0,0,0},{.3,0,0,0},{.1,0,0,0},{.2,0,0,0}};
  float times[400];
  FastNoise noise;
  for(int i=0; i < 400; i++){
    times[i] = Random::range(0.f,10.f);
  }
  int demoNumber = 9;
  bool trippy = false;
  while (!glfwWindowShouldClose(mainWin.window)) {
    glfwPollEvents();
   ImGui_ImplGlfwGL3_NewFrame();


    ImGui::ColorEdit4(   "clear_color", (float *)&clear_color); // Edit 3 floats representing a color
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",  1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    ImGui::InputInt("DemoNumber", &demoNumber,0,4);

    glClearColor(SPLAT4(clear_color));
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glViewport(0,0,mainWin._height, mainWin._height);

    bool redraw = false, clear = false;

// interlace a secondary buffer to save the final image?
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
        buff.begin(clear);
        if(redraw) {
          flame.use();
          glUniform4fv(glGetUniformLocation(flame.ID, "u_adj"), 4, (float*) &params[0]);
          ctx->render();
        }
        glViewport(0,0,mainWin._height, mainWin._height);
        buff.end();
        break;
      case 3:
        for(int i=0; i < 4; i++){
          ImGui::PushID(i);
          ImGui::ColorEdit4("arg", (float*)&(params[i]));
          ImGui::PopID();
        }
        ImGui::Checkbox("Trip out", &trippy);
        if(!trippy){
          flame.use();
          glUniform4fv(glGetUniformLocation(flame.ID, "u_adj"), 4, (float*) &params[0]);
          ((ProcessingGL*) ctx)->setMode(GL_POINTS);
        }
        else{
          basic.use();
          ((ProcessingGL*) ctx)->setMode(GL_TRIANGLES);
        }
        p.update(ctx, (float) glfwGetTime());
        break;
      case 4: {
        basic.use();
        ((ProcessingGL*) ctx)->setMode(GL_TRIANGLES);
        ctx->clear();
        s.render(ctx);
        ctx->flush();
        ctx->render();
        break;
      }
      case 5: {
        bool  redraw = f.imSettings();
        buff.begin(redraw);

        if(f.needsFrame()){
          f.render(ctx);
          basic.use();
          ((ProcessingGL*) ctx)->setMode(GL_TRIANGLES);
          ctx->render();
        }

        glViewport(0,0,mainWin._height, mainWin._height);
        buff.end();
        break;
      }
      case 6:{ // procmap
        bool redraw = map.imSettings();
        redraw |= Util::load_json(map, "procgen_map.json", ImGui::Button("SaveMap"), ImGui::Button("LoadMap"));


        if(redraw){
          map.render((ProcessingGL*) ctx);
        }
        basic.use();
        ((ProcessingGL*) ctx)->setMode(GL_TRIANGLES);
        ctx->render();
        break;
      }
      case 7:{ // particle demo

        part_ctx->clear();
        static vec3 pos{0,0,0};
        static float size = .03;
        static float texsize = .4;
        static int numpts = 0;
        float ySkew = .3;
        static vec3 vel{.2,.1, 0};
        ImGui::DragFloat3("Position", (float*)&pos, 0, 1);
        ImGui::DragFloat3("vel", (float*)&vel, 0, 0.1);
        ImGui::DragFloat("size", &size, 0,1);
        ImGui::DragFloat("texsize", &texsize, 0,1);
        ImGui::DragInt("numpts", &numpts, 1, 400);

        auto fibonacci = [](int k, const int n){
          // https://stackoverflow.com/questions/9600801/evenly-distributing-n-points-on-a-sphere
          float offset = 2.0f/n;
          float golden_angle = 3.1415f * (3 - sqrt(5.f));

          float r = sqrt(1.0f * k) / sqrt(1.0f * n);
          float phi = k * golden_angle;  // = ((i + rnd) % samples) * increment
          float x = cos(phi)*r;
          float y = sin(phi)*r;
          return vec3{x,y,0};
        };
        Particle_Vertex::particle_data dat = {{.5,.5}, texsize, size};

        for(int i=0; i < numpts; i++){
          vec3 spewpos = fibonacci(i, numpts);
          part_ctx->indexVert({pos + spewpos, dat});
        }
        part_ctx->flush();
        part_ctx->setMode(GL_POINTS);
        particle.use();
        m_tonemap.use(GL_TEXTURE0);
        part_ctx->render();
        break;
      }
      case 8: { // pendulum
        ctx->clear();
        ((ProcessingGL*) ctx)->setMode(GL_TRIANGLES);
        drawPendulum(ctx, redraw, clear); // todo: maek it a return value, or put buffer inside of this function
        buff.begin(clear);
        if(redraw) {
          basic.use();
          ctx->flush();
          ctx->render();
        }
        glViewport(0,0,mainWin._height, mainWin._height);
        buff.end();

        break;
      }
      case 9: { // pendulum
        ctx->clear();
        ((ProcessingGL*) ctx)->setMode(GL_TRIANGLES);
        drawNoise(ctx, redraw, clear); // todo: maek it a return value, or put buffer inside of this function
        buff.begin(clear);
        if(redraw) {
          basic.use();
          ctx->flush();
          ctx->render();
        }
        glViewport(0,0,mainWin._height, mainWin._height);
        buff.end();

        break;
      }
      default:
        break;
    }


    processInput(mainWin.window);

    t += .01f; // TODO: real deltatime

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
