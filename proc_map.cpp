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
  return redraw;
}
