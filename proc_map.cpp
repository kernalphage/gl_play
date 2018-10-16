//
// Created by matt on 10/14/18.
//

#include <imgui.h>
#include "proc_map.hpp"
#include "Processing.hpp"
#include "hexlib.hpp"
#include <FastNoise/FastNoise.h>




void proc_map::render(ProcessingGL *ctx) {
  float dx = 2.0/ m_chunksize;
  Layout l(layout_pointy, vec2{dx/2, dx/2}, vec2{-1,-1});
  FastNoise noise;
  noise.SetSeed(m_seed );
  noise.SetFrequency(m_frequency);
  ctx->clear();

  for(int x=0; x < m_chunksize; x++){
    for(int y = 0; y < m_chunksize; y++){
      Hex pos(x-y/2, y);
      auto p = hex_to_pixel(l, pos);
      float ic = noise.GetPerlinFractal(p.x, p.y) * .5f + .5f;
      if( ic < waterThreshhld){
        auto color= w.sample(Util::rangeMap(ic, 0, waterThreshhld, 0,1) );
        ctx->ngon(p, dx/2, 6, color,color);

      } else {
        auto color= g.sample(Util::rangeMap(ic, waterThreshhld,1 , 0,1) );
        ctx->ngon(p, dx/2, 6, color,color);

      }
    }
  }
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
  static bool p_open = true;
  const float DISTANCE = 10.0f;
  static int corner = 0;
  float mousex = ImGui::GetIO().MousePos.x, mousey = ImGui::GetIO().MousePos.y;
  /// ImVec2 window_pos = ImVec2((corner & 1) ? ImGui::GetIO().DisplaySize.x - DISTANCE : DISTANCE, (corner & 2) ? ImGui::GetIO().DisplaySize.y - DISTANCE : DISTANCE);
  ImVec2 window_pos = ImVec2(mousex, mousey );

  ImVec2 window_pos_pivot = ImVec2(mousex > ImGui::GetIO().DisplaySize.x/2? 1 : 0,
                                   mousey > ImGui::GetIO().DisplaySize.y/2? 1 : 0);
  ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
  ImGui::SetNextWindowBgAlpha(0.3f); // Transparent background
  if (ImGui::Begin("Example: Fixed Overlay", &p_open, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_AlwaysAutoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoSavedSettings|ImGuiWindowFlags_NoFocusOnAppearing|ImGuiWindowFlags_NoNav))
  {
    ImGui::Text("Simple overlay\nin the corner of the screen.\n(right-click to change position)");
    ImGui::Separator();
    ImGui::Text("Mouse Position: (%.1f,%.1f)", ImGui::GetIO().MousePos.x, ImGui::GetIO().MousePos.y);

    ImGui::End();
  }
  return redraw;
}
