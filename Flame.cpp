#include "Flame.hpp"
#include "imgui.h"
#include "Random.hpp"
#include <tuple>

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
