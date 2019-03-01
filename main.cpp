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
#include "proc/DifferentialGrowth.hpp"
#include "Processing.hpp"
#include "RenderTarget.hpp"
#include "sketches.hpp"
#include "Window.hpp"
#include <fmt/format.h>
#include <langinfo.h>
#include <rapidjson/rapidjson.h>
#include "proc/cached_flowmap.hpp"
#include "proc/Superformula.hpp"


using namespace std;
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Material* currentMaterial;

enum class PostMode{Buffer, NoBuffer};
using ProcUpdate = void (*)(Processing*, bool&, bool&, int, int);
struct procFunction{ 
  string name;
  ProcUpdate update;
  GLuint mode;
  Material* mat;
  PostMode postProcessing = PostMode::NoBuffer;
};


void drawDiffGrow(Processing* ctx, bool&redraw, bool&clear, int curFrame, int maxFrames){
  static DifferentialGrowth diffGrow;


  static bool alwaysClear = false;
  ImGui::Checkbox("always clear", &alwaysClear);
  clear = diffGrow.imSettings();
  clear = clear || alwaysClear;

  redraw = true;
  diffGrow.update();
  diffGrow.render(ctx);
}



TriBuilder tri;
float t = 0;
void drawTriangle(Processing* ctx, bool&redraw, bool&clear, int curFrame, int maxFrames){
  redraw = true;
   if(tri.imSettings()){
    clear = true;
  }
    tri.triangulate(t, ctx);
}

Streamline s(1,1);
void drawStream(Processing* ctx, bool& redraw, bool& clear, int curFrame, int maxFrames){
  clear  = s.imSettings();
  redraw = true;    
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

vec4 params[4]{{.2,0,0,0},{.3,0,0,0},{.1,0,0,0},{.2,0,0,0}};
void drawFlame(Processing* ctx, bool& redraw, bool& clear, int curFrame, int maxFrames){
  for(int i=0; i < 4; i++){
    ImGui::PushID(i);
    ImGui::ColorEdit4("arg", (float*)&(params[i]));
    ImGui::PopID();
  }
  Flame::do_flame(ctx, redraw, clear);
  glUniform4fv(glGetUniformLocation(currentMaterial->ID, "u_adj"), 4, (float*) &params[0]);
}

Plotter p;
void drawParticles(Processing* ctx, bool& redraw, bool& clear, int curFrame, int maxFrames){
  for(int i=0; i < 4; i++){
    ImGui::PushID(i);
    ImGui::ColorEdit4("arg", (float*)&(params[i]));
    ImGui::PopID();
  }
  glUniform4fv(glGetUniformLocation(currentMaterial->ID, "u_adj"), 4, (float*) &params[0]);
  redraw = true;
  p.update(ctx, (float) glfwGetTime());
}

void drawParticles2(Processing* ctx, bool& redraw, bool& clear, int curFrame, int maxFrames){
  redraw = true;
  p.update(ctx, (float) glfwGetTime());
}


// settings
Processing *ctx;

static void error_callback(int error, const char* description)
{
  fprintf(stderr, "Error %d: %s\n", error, description);
}

int main() {
  Window mainWin;
  mainWin.init(1000,1000);

  glfwSetErrorCallback(error_callback);
  glEnable(GL_DEBUG_OUTPUT);

  // build and compile our shaders
  Material basic{"shaders/basic.vert", "shaders/basic.frag", true};
  Material flame{"shaders/basic.vert", "shaders/basic.frag", true, "shaders/flame.geom"};
  Material particle{"shaders/particle.vert", "shaders/textured.frag", true, "shaders/particle.geom"};
  Texture m_tonemap;
  m_tonemap.load("particle.png");
  particle.setInt("textured.frag", 0);

  // Setup common render components 
  ctx = new ProcessingGL_t<UI_Vertex, vec4>{};
  auto* part_ctx = new ProcessingGL_t<Particle_Vertex, Particle_Vertex::particle_data>{};
  RenderTarget buff(1000,1000);
  buff.init();


  // Setup ImGui binding
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |=  ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad
  // Controls
  ImGui_ImplGlfwGL3_Init(mainWin.window, true);

  vec4 clear_color{0.00f, 0.0f, 0.0f, 1.00f};

  s.stream_point({0.5f,0.5f});
  for(int i=0; i < 100; i++){
    vec2 p = Random::random_point({-1,-1}, {1,1});
    s.stream_point(p);
  }

  Cellular cells;
  Blob b{{0,0}, .5f};


/// Game loop 
  while (!glfwWindowShouldClose(mainWin.window)) {
    glfwPollEvents();
    ImGui_ImplGlfwGL3_NewFrame();

    ImGui::ColorEdit4("clear_color", (float *)&clear_color);
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",  1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    glClearColor(SPLAT4(clear_color));
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0,0,mainWin._height, mainWin._height);

    procFunction functions[]= {
      //{"triangles", drawTriangle, GL_TRIANGLES, &basic, PostMode::NoBuffer},
      {"stream" , drawStream, GL_TRIANGLES, &basic, PostMode::NoBuffer},
        {"DiffGrowth", drawDiffGrow, GL_TRIANGLES, &basic, PostMode ::Buffer},
        {"flame", drawFlame, GL_POINTS, &flame, PostMode::Buffer},
      {"flameplotter", drawParticles, GL_POINTS, &flame, PostMode::NoBuffer},
      {"plotter", drawParticles2, GL_TRIANGLES, &basic, PostMode::NoBuffer},
      {"curve", do_curve, GL_TRIANGLES, &basic, PostMode::Buffer},
      {"flowmap", drawFlowmap, GL_TRIANGLES, &basic, PostMode::Buffer},
      {"procmap", drawProcmap ,GL_TRIANGLES, &basic, PostMode::NoBuffer},
      {"Pendulum", drawPendulum, GL_TRIANGLES, &basic, PostMode::Buffer},
      {"noise", drawNoise, GL_TRIANGLES, &basic, PostMode::Buffer},
      {"light", drawLight, GL_TRIANGLES, &basic, PostMode::Buffer},

    // {"cells", PROC_FORWARD(cells.render), GL_TRIANGLES, &basic}
    };

    const int numFunctions = 8;
    {
      // setup current view 
      bool redraw = false, clear = false;
      static int curfn_idx = 0; 
      auto curfn = functions[curfn_idx];
      ImGui::LabelText("curfn", "%s", curfn.name.c_str());
      ImGui::SliderInt("function", &curfn_idx, 0, numFunctions);
      curfn_idx = glm::min(curfn_idx, numFunctions);
      currentMaterial = curfn.mat;    

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
      static glm::vec3 translate;
      static float scale = 1;
      ImGui::SliderFloat3("translate", (float*)(&translate.x), -6.28, 6.28);
      ImGui::SliderFloat("translate", &scale, 0, 10);
      clear |= ImGui::Button("Clear");
      glm::mat4 worldTransform = glm::mat4(1.0f);
      worldTransform *= glm::translate(translate);
      worldTransform *= glm::scale(vec3{scale,scale,scale});
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

        // Draw 
        if(curfn.postProcessing == PostMode::Buffer){
          buff.begin(clear);
        }
        if (redraw) {
          curfn.mat->use();
          curfn.mat->setMat4x4("worldTransform", worldTransform);
          ctx->flush();
          ctx->render();
        }
        glViewport(0, 0, mainWin._height, mainWin._height);
        if(curfn.postProcessing== PostMode::Buffer){
          buff.end();
        }
        if(clear){
          curFrame = (curFrame + 1 ) % maxFrames;
        }
      }

      if (glfwGetKey(mainWin.window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
        glfwSetWindowShouldClose(mainWin.window, true);
      }
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
