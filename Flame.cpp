#include "Flame.hpp"
#include "imgui.h"
#include "Random.hpp"
#include <tuple>
#include <vector>
#include "Processing.hpp"
using namespace std;


#define FLAME_LISTFN(x) { #x, Flame::x }

const char* flame_listbox_items[] = {
	"linear" ,
	"sinus" ,
	"sphere" ,
	"swirl",
  "horseshoe",
  "polar",
  "handkerchief",
};

const Flame::FolderFunction flame_function_items[] = {
    Flame::linear,
    Flame::sinus,
    Flame::sphere,
    Flame::swirl,
    Flame::horseshoe,
    Flame::polar,
    Flame::handkerchief,
};

void Flame::randomInit(){
  offset = Random::random_point(vec2{-.2f,-.2f},vec2{.2f,.2f});
  vars = Random::random_point(vec4{-2,-2,-2,-2},vec4{2,2,2,2});
  strength = Random::range(.2f, .7f);
  type = Random::range(1, IM_ARRAYSIZE(flame_listbox_items));
}

bool Flame::imSettings(int id){
  bool redraw = false;

  redraw |= ImGui::SliderFloat2("offset", (float*)&offset, -2,2);
  redraw |= ImGui::SliderFloat4("vars", (float*)&vars, -2,2);
  redraw |= ImGui::SliderFloat("strength", &strength, 0, 1);

  redraw |= ImGui::ListBox("FlameStyle", &type, flame_listbox_items, IM_ARRAYSIZE(flame_listbox_items), 4);
  return redraw;
}

vec2 Flame::fn(vec2 pt) {
  return mix( pt, flame_function_items[type](pt + offset, vars), strength);
}


void Flame::do_flame(Processing * ctx, bool& _r, bool& _c){
  // TODO: abstract the heatmap from the thing that draws on the heatmap

  static int seed = 12;
  static int numPts = 122;
  static int numLayers = 72;
  static int num_iterations = 2;
  static int num_samples = 5; // Number of samples to place after reaching num_iterations
  static int curLayer = 0;
  static float startScale = 3;
  static vec2 endOffset = vec2(0,0);
  static float endScale = 1.f;
  static bool symmetrical = false;
  static float sampleSize = .01;
  static vector<Flame> ff(5);

  static vec4 color{1.f, 1.f, 0.f, 1};

  bool redraw = false;
  STATIC_DO_ONCE(redraw = true;);
  bool reseed = false;
  STATIC_DO_ONCE(reseed = true;);
  // Typically we would use ImVec2(-1.0f,0.0f) to use all available width, or ImVec2(width,0.0f) for a specified width. ImVec2(0.0f,0.0f) uses ItemWidth.
  ImGui::ProgressBar((float) curLayer / numLayers, ImVec2(0.0f,0.0f));
  ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
  ImGui::Text("Progress Bar");

  redraw |= reseed = ImGui::InputInt("Seed", &seed);
  redraw |= ImGui::SliderInt("numPts", &numPts, 2, 5500);
  ImGui::SliderInt("Layers", &numLayers, 2, 400);
  redraw |= ImGui::SliderInt("NumIterations", &num_iterations, 2,99);
  redraw |= ImGui::SliderInt("NumSamples", &num_samples, 1, 20);
  redraw |= ImGui::SliderFloat("startScale", &startScale, .05, 5);
  redraw |= ImGui::SliderFloat2("endOffset", (float*)&endOffset, -2, 2 );
  redraw |= ImGui::SliderFloat("endScale", &endScale, .05, 5);
  redraw |= ImGui::Checkbox("Symmetrical", &symmetrical);
  redraw |= ImGui::SliderFloat("sampleSize", &sampleSize, .0001, .05);

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
  //cout<<"Layer "<< curLayer<<endl;
  curLayer++;
  ctx->clear();
  if(redraw || reseed){
    curLayer = 0;
  }
  if(reseed){
    for(auto&f:ff){ f.randomInit();};
  }
  Random::seed(seed + curLayer);

  int fnSize = ff.size();
  for(int i=0; i < numPts; i++){
    vec2 p = Random::random_point({-startScale, -startScale}, {startScale, startScale});
    vec4 xcolor = vec4(1.f,1.f, p.x, 1 );
    vec4 aColor = vec4(xcolor.x, xcolor.y, xcolor.z, 0);

    for(int i =0; i < num_iterations + num_samples; i++) {
      if(symmetrical && Random::range(0,10) <= 5){
        p = vec2(-p.x, p.y);
      }
      int idx = Random::range(0, fnSize);
      idx = idx % fnSize;
      auto f = ff[glm::min(idx, fnSize)];
      p = f.fn(p);

      if(i > num_iterations){
        vec3 pp{p.x,p.y, 0};
        pp *= endScale;
        pp += vec3(endOffset.x, endOffset.y, 0);
        float r = sampleSize;
        float thickness = r;
        const float tau = 6.282;
        float dTheta = (tau) / 4;

        auto cPos = [=](float t, float radius){ return vec3{sin(t), cos(t), 1} * radius + pp;};
        for (float theta = dTheta; theta <= (tau); theta += dTheta) {
          ctx->quad(UI_Vertex{cPos(theta, sampleSize), aColor},
                    UI_Vertex{cPos(theta + dTheta, sampleSize), aColor},
                    UI_Vertex{cPos(theta + dTheta, 0), xcolor},
                    UI_Vertex{cPos(theta, 0), xcolor});
        }
      }
    }

  }

  ctx->flush();
  _r = true;
  _c = curLayer == 0;
  return ;
}
