//
// Created by matt on 10/14/18.
//

#include <imgui.h>
#include "proc_map.hpp"
#include "Processing.hpp"
#include "hexlib.hpp"
#include <FastNoise/FastNoise.h>
#include <iostream>
#include "Window.hpp"

Hex m_curHex{0,0};

void proc_map::render(ProcessingGL *ctx) {
  float dx = 1.0f/ m_chunksize;
  Layout l = Layout(layout_pointy, vec2{dx, dx}, vec2{-1, -1});
  FastNoise noise;
  noise.SetSeed(m_seed );
  noise.SetFrequency(m_frequency);
  ctx->clear();

  for(int x=0; x < m_chunksize * 1.5; x++){
    for(int y = 0; y < m_chunksize * 1.5; y++){
      Hex pos(x-y/2, y);
      auto p = hex_to_pixel(l, pos);

      if(m_curHex == pos || x==0 && y == 0){
        ctx->ngon(p, dx, 6, {1,0,0,1}, {1,0,0,0});
        continue;
      }

      float ic = noise.GetPerlinFractal(p.x, p.y) * .5f + .5f;
      if( ic < waterThreshhld) { // water 
        auto color= w.sample(Util::rangeMap(ic, 0, waterThreshhld, 0,1) );
        ctx->ngon(p, dx, 6, color,color);

      } else { // ground
        auto color= g.sample(Util::rangeMap(ic, waterThreshhld,1 , 0,1) );
        ctx->ngon(p, dx, 6, color,color);
      }
    }
  }
  vec4 c{1,0,1,.4};
  ctx->quad({{-.99,-.99,0},c}, {{-.99,.99,0},c}, {{.99,.99,0},c}, {{.99,-.99,0},c});
  ctx->flush();
}

bool proc_map::imSettings() {
  bool redraw = false;
  STATIC_DO_ONCE(redraw = true);

  redraw |= ImGui::InputInt("Seed", &m_seed);
  redraw |= ImGui::SliderInt("chunksize", &m_chunksize, 2, 100);
  redraw |= ImGui::SliderFloat("frequency", &m_frequency, .01, 5);
  redraw |= ImGui::SliderFloat("watterThreshhold", &waterThreshhld, 0,1);
  redraw |= w.imSettings("WaterColor");
  redraw |= g.imSettings("grounddColor");


  // calculate member variables
  float dx = 1.0f/ m_chunksize;
  Layout l = Layout(layout_pointy, vec2{dx, dx}, vec2{-1,-1});

  static bool p_open = true;
  const float DISTANCE = 10.0f;
  static int corner = 0;
  float mousex = (ImGui::GetIO().MousePos.x), mousey = (ImGui::GetIO().MousePos.y);
  if(mousex > ImGui::GetIO().DisplaySize.x/2){
    mousex -= 5;
  }
  else{
  mousex -= 5;
  }
  vec2 calcPos{mousex/ImGui::GetIO().DisplaySize.y, (ImGui::GetIO().DisplaySize.y - mousey) / ImGui::GetIO().DisplaySize.y};
  calcPos = calcPos * 2 - vec2{ 1,1};

  auto hexPix = hex_round(pixel_to_hex(l, calcPos));
  m_curHex = hexPix;
  /// ImVec2 window_pos = ImVec2((corner & 1) ? ImGui::GetIO().DisplaySize.x - DISTANCE : DISTANCE, (corner & 2) ? ImGui::GetIO().DisplaySize.y - DISTANCE : DISTANCE);
  ImVec2 window_pos = ImVec2(mousex, mousey );

  ImVec2 window_pos_pivot = ImVec2(mousex > ImGui::GetIO().DisplaySize.x/2? 1 : 0,
                                   mousey > ImGui::GetIO().DisplaySize.y/2? 1 : 0);

  ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
  ImGui::SetNextWindowBgAlpha(0.3f); // Transparent background
  if (ImGui::Begin("Example: Fixed Overlay", &p_open, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_AlwaysAutoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoSavedSettings|ImGuiWindowFlags_NoFocusOnAppearing|ImGuiWindowFlags_NoNav))
  {
    ImGui::Text("Simple overlay\nin the corner of the screen");
    ImGui::Separator();
    ImGui::Text("hex  Position: (%d,%d)", hexPix.q, hexPix.r);
    ImGui::Text("Mouse Position: (%f,%f)", calcPos.x, calcPos.y);

    ImGui::End();
  }

  redraw |= do_serialize();
  return redraw;
}

bool  proc_map::do_serialize(){
  if(ImGui::Button("Save JSON")){
    DonerSerializer::CJsonSerializer serializer;
    serializer.Serialize(*this);
    std::string result = serializer.GetJsonString();
    std::cout<<result <<std::endl;
  }
  return false;
}
