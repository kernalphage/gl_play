#include "Definitions.hpp"
#include "imgui.h"
#include "imgui_impl/glfw_gl3.h"
#include "Material.hpp"
#include "Plotter.hpp"
#include "proc/Blob.hpp"
#include "proc/Cellular.hpp"
#include "proc/Flame.hpp"
#include "proc/flowmap.hpp"
#include "proc/proc_map.hpp"
#include "proc/Streamline.hpp"
#include "proc/Streamline.hpp"
#include "proc/Triangulate.hpp"
#include "Processing.hpp"
#include "RandomCache.hpp"
#include "RenderTarget.hpp"
#include "sketches.hpp"
#include "Window.hpp"
#include <fmt/format.h>
#include <langinfo.h>
#include <rapidjson/rapidjson.h>

using namespace std;
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

using ProcUpdate = void (*)(Processing*, bool&, bool&, int, int);
struct procFunction{ 
  string name;
  ProcUpdate update;
  GLuint mode;
  Material* mat;
  bool postProcessing = false;
};


TriBuilder tri;
float t = 0;
void drawTriangle(Processing* ctx, bool&redraw, bool&clear, int curFrame, int maxFrames){
  redraw = true;
   if(tri.imSettings()){
    clear = true;
    tri.triangulate(t, ctx);
  }
}

 Streamline s(1,1);
void drawStream(Processing* ctx, bool& redraw, bool& clear, int curFrame, int maxFrames){
  redraw = s.imSettings();
  clear = redraw;        
  s.render(ctx);
}

flowmap f;
void drawFlowmap(Processing* ctx, bool& redraw, bool& clear, int curFrame, int maxFrames){
      clear = f.imSettings();
      redraw = f.needsFrame();
      if(redraw){
        f.render(ctx);
      }
}

proc_map procmap;
void drawProcmap(Processing* ctx, bool& redraw, bool& clear, int curFrame, int maxFrames){
  redraw = true;
  clear = procmap.imSettings();
  clear |= Util::load_json(procmap, "procgen_map.json", ImGui::Button("SaveMap"), ImGui::Button("LoadMap"));
  if(redraw){
    procmap.render((ProcessingGL*) ctx); 
  }
}
// settings
Processing *ctx;
Plotter p;


void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
}

static void error_callback(int error, const char* description)
{
  fprintf(stderr, "Error %d: %s\n", error, description);
}
void drawLight(Processing* ctx, bool& redraw, bool& clear, int curFrame, int maxFrames){
  static float angle = 0;
  static float spread = .2;
  static int numrays = 500;
  static int seed =0;
  static int numMirrors = 32;
  static float minDist = .2;
  static float thickness = 0.001;
static vector<vec2> mirrorPts;

STATIC_DO_ONCE(clear = true;);
  clear |= ImGui::InputInt("seed", &seed);
  clear |= ImGui::SliderFloat("spread", &spread, 0, 6.28);
  clear |= ImGui::SliderFloat("angle", &angle, 0, 6.28);
  clear |= ImGui::SliderInt("numrays", &numrays, 0, 10240);
  clear |= ImGui::SliderInt("numMirrors", &numMirrors, 2, 19);
  clear |= ImGui::SliderFloat("minDist", &minDist, 0, 1);

  if(clear){
  Random::seed(seed);
  mirrorPts.clear();
  for(int i=0; i < numMirrors; i++){
    mirrorPts.push_back(Random::gaussPoint() * .4f) ;
  }

  std::sort(mirrorPts.begin(), mirrorPts.end(), [](vec2 a, vec2 b){ return glm::length(a) < glm::length(b);});
}
/*
// Connect close points
    for(int mirror = 0; mirror < mirrorPts.size() - 1; mirror+=2){
      auto pta = mirrorPts[mirror];
      for(int b = mirror+1; b < mirrorPts.size() - 1; b++){
        auto ptb = mirrorPts[b];
        if(glm::distance(pta, ptb) < minDist){
       ((ProcessingGL*) ctx)->line(vec3{SPLAT2(pta),0}, vec3{SPLAT2(ptb), 0}, {1,.4,0,numrays}, thickness);
      }
    }
  }
  */
   for(int mirror = 0; mirror < mirrorPts.size() - 1; mirror+=2){
      auto pta = mirrorPts[mirror];
      auto ptb = mirrorPts[mirror + 1];  
      ((ProcessingGL*) ctx)->line(vec3{SPLAT2(pta),0}, vec3{SPLAT2(ptb), 0}, {1,.4,0,numrays}, thickness);
    
    }

    float fudge =  Random::nextGaussian() * .001;
    float di = 1.0f / numrays;
  for(int i=0; i < numrays; i++){

    float min_dist = 1000;
    Geo::ray minray;
    float rayAngle = fudge+  angle +  i / (numrays * spread);
    vec2 raydir{sin(rayAngle), cos(rayAngle)};
    
    for(int mirror = 0; mirror < mirrorPts.size() - 1; mirror+=2){
      auto r = Geo::rayBounce({{0,0}, raydir}, mirrorPts[mirror], mirrorPts[mirror+1]);
      if(std::get<0>(r) < min_dist){
        min_dist = std::get<0>(r);
        minray = std::get<1>(r);
      }
    }
    if(min_dist < 1000){
       auto newdir = minray.p + normalize(minray.d);
       ((ProcessingGL*) ctx)->line({minray.p.x, minray.p.y, i * di}, {newdir.x, newdir.y, i * di}, {0, 1, 0, 1}, thickness);
       ((ProcessingGL*) ctx)->line(vec3{0,0,0}, {SPLAT2(minray.p), i * di}, {0,0,1,1}, 0.001);
    }
    else{
       ((ProcessingGL*) ctx)->line(vec3{0,0,0}, 20 * vec3(SPLAT2(raydir), i * di), {1,0,0,1}, thickness);
    } 
  }
  redraw = true;
}

void drawNoise(Processing* ctx, bool& redraw, bool& clear, int curFrame, int maxFrames){
  STATIC_DO_ONCE(clear = true);
  static FastNoise n;
  static RandomCache rx{1000, 2, [](vec2 pos){return n.GetValueFractal(pos.x, pos.y, 0);}};
  static RandomCache ry{1000, 2, [](vec2 pos){return n.GetValueFractal(pos.x, pos.y, 100);}};
  static float noisefreq = 8.31511;
  static int numpts = 900;
  static float maxVel = .005;
  static vector<vec4> pts;
  static float angleVel = 1.451;
  static float noiseScale = .2f;
  static int numIterations = 1;
  static int curIterations = 0;
  static int totalIterations = 1000;
  ImGui::ProgressBar((float) curIterations / totalIterations, ImVec2(0.0f,0.0f));
  ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
  ImGui::Text("frame progress");
  bool reNoise = ImGui::SliderFloat("frequency", &noisefreq, .01, 15);
  STATIC_DO_ONCE(reNoise = true);
  reNoise|= ImGui::SliderFloat("timeScale", &noiseScale, 0.01, 1);

  //reNoise = true;
  if(reNoise || curIterations >= totalIterations){
    //totalTime += .01;
    const float totalTime = (float) 6.28f * curFrame / maxFrames;
    const float sT = sin(totalTime)*noiseScale;
    const float cT = cos(totalTime)*noiseScale;
    n.SetFrequency(noisefreq);

    rx.reseed(1, [=](vec2 pos){return n.GetValueFractal(pos.x, pos.y + sT, cT);});
    ry.reseed(1, [=](vec2 pos){return n.GetValueFractal(pos.x, pos.y + sT,100+ cT);});
    clear = true;
  }
  ImGui::LabelText("cur iterations", "cur iterations %d", curIterations);
  clear |= ImGui::SliderInt("numPts", &numpts, 10, 5000);
  clear |= ImGui::SliderFloat("velocity", &maxVel, 0,.1);
  clear |= ImGui::SliderFloat("angleVel", &angleVel, 0,20);
  ImGui::SliderInt("num iterations", &numIterations, 1, 100);
  ImGui::SliderInt("total Iterations" , &totalIterations, 100, 100000);

  if(clear){
    pts.clear();
    curIterations= 0;
  }
  redraw = true;
  curIterations += numIterations;
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
      vec4 color = lerp(vec4{.5,.9, 0, 1}, vec4{.2,.6,.1,1}, abs(ex));
      color.x =abs(ex);
      color.y =abs(wy);
      ctx->ngon(pos, maxVel, 6, vec4{SPLAT3(color), 0}, color);
    }
  }
}

void drawPendulum(Processing* ctx, bool& redraw, bool& clear, int curFrame, int maxFrames){
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
        ctx->ngon(next, .004,8, vec4{1,1,1,0}, vec4{1,1,1,1} );
      }
      cur = next;
//      ctx->line(cur, next);
    }
    float r = abs(cos(time));
   // ctx->spline(points, vec4{r,.1, .5, 1}, vec4{r,.1,.5, .0}, .001);
  }
}

int main() {
  Window mainWin;
  mainWin.init(1000,1000);

  glfwSetErrorCallback(error_callback);
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

  //s.stream_point({0.5f,0.5f});
  for(int i=0; i < 100; i++){
    vec2 p = Random::random_point({0,0}, {1,1});
    s.stream_point(p);
  }

  Cellular cells;
  Blob b{{0,0}, .5f};
  RenderTarget buff(1000,1000);
  buff.init();
  vec4 params[4]{{.2,0,0,0},{.3,0,0,0},{.1,0,0,0},{.2,0,0,0}};

  bool trippy = false;
  while (!glfwWindowShouldClose(mainWin.window)) {
    glfwPollEvents();
   ImGui_ImplGlfwGL3_NewFrame();

    ImGui::ColorEdit4("clear_color", (float *)&clear_color);
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",  1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);


    glClearColor(SPLAT4(clear_color));
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glViewport(0,0,mainWin._height, mainWin._height);

    bool redraw = false, clear = false;
    const int demoNumber = 0;
    switch(demoNumber){
      case 0:
        break;
      case 1:
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
        break;
      }
      case 5: {
        break;
      }
      case 6:{ // procmap
        break;
        }
      case 7:{ // particle demo

        part_ctx->clear();
        static vec3 pos{0,0,0};
        static float size = .03;
        static float texsize = .4;
        static int numpts = 0;
        static vec3 vel{.2,.1, 0};
        ImGui::DragFloat3("Position", (float*)&pos, 0, 1);
        ImGui::DragFloat3("vel", (float*)&vel, 0, 0.1);
        ImGui::DragFloat("size", &size, 0,1);
        ImGui::DragFloat("texsize", &texsize, 0,1);
        ImGui::DragInt("numpts", &numpts, 1, 400);

        Particle_Vertex::particle_data dat = {{.5,.5}, texsize, size};

        for(int i=0; i < numpts; i++){
          vec3 spewpos = Geo::fibonacci(i, numpts);
          part_ctx->indexVert({pos + spewpos, dat});
        }
        part_ctx->flush();
        part_ctx->setMode(GL_POINTS);
        particle.use();
        m_tonemap.use(GL_TEXTURE0);
        part_ctx->render();
        break;
      }

      default:
        break;
    }

    procFunction functions[]= {
      //{"triangles", genTriangle, GL_TRIANGLES, &basic},
      //{"flame", Flame::do_flame, GL_POINTS, &basic},
      //{"flame", }
      //{"stream" , drawStream, GL_TRIANGLES, &basic, false},
      {"curve", do_curve, GL_TRIANGLES, &basic, true},
      {"flowmap", drawFlowmap, GL_TRIANGLES, &basic, true},
      {"procmap", drawProcmap ,GL_TRIANGLES, &basic, false },
      {"Pendulum", drawPendulum, GL_TRIANGLES, &basic, true},
      {"noise", drawNoise, GL_TRIANGLES, &basic, true},
      {"light", drawLight, GL_TRIANGLES, &basic, true},

    // {"cells", PROC_FORWARD(cells.render), GL_TRIANGLES, &basic}
    };
    const int numFunctions = 5;

    {
      static int curfn_idx = 0; 
      auto curfn = functions[curfn_idx];
      ImGui::LabelText("curfn", "%s", curfn.name.c_str());
      ImGui::SliderInt("function", &curfn_idx, 0, numFunctions);
      curfn_idx = glm::min(curfn_idx, numFunctions);

      static bool animating = false;
      static string animTimestamp;
      static int curFrame = 0;
      static int maxFrames = 200;
      ImGui::ProgressBar((float) curFrame / maxFrames, ImVec2(0.0f,0.0f));
      ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
      ImGui::Text("animation progress");
      ImGui::SliderInt("frame count", &maxFrames, 1, 1000);
      if(ImGui::Button("Start Animation")){
        animating = true;
        animTimestamp = Util::timestamp(0);
        curFrame = 0;
      }

      // Viewport 
      static glm::vec3 rotation;
      ImGui::SliderFloat3("rotation", (float*)(&rotation.x), 0, 6.28);
      clear |= ImGui::Button("Clear");
      glm::mat4 worldTransform = glm::mat4(1.0f);
      worldTransform *= glm::orientate4(rotation);

      // update loop
      ctx->clear();
      ((ProcessingGL*) ctx)->setMode(curfn.mode);
      curfn.update(ctx, redraw, clear, curFrame, maxFrames);
      if(animating && clear && curFrame < maxFrames){
        auto filename = fmt::format("anim/frame{0}{1}_{2:04d}.png", curfn.name,  animTimestamp, curFrame);
        curFrame ++;

        // TODO: Save out frames for procs that don't use a buffer
        // interlace a secondary buffer to save the final image?
        buff.save(filename.c_str());
        buff.begin(true);
        buff.end();
      } else {
        if(curfn.postProcessing){
          buff.begin(clear);
        }
        if (redraw) {
          curfn.mat->use();
          curfn.mat->setMat4x4("worldTransform", worldTransform);
          ctx->flush();
          ctx->render();
        }
        glViewport(0, 0, mainWin._height, mainWin._height);
        if(curfn.postProcessing){
          buff.end();
        }
        if(clear){
          curFrame = (curFrame + 1 ) % maxFrames;
        }
      }
      processInput(mainWin.window);
    }

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
