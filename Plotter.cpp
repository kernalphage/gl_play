//
// Created by matt on 4/25/18.
//

#include "Plotter.hpp"
#include "Processing.hpp"
#include "imgui.h"


const char* listbox_items[] = { "random", "circular", "sinwave", "image", "wipe"};
  
bool LayerSettings::imSettings(int id){
    ImGui::PushID(id);
    bool recolor = false;
    recolor |= ImGui::ColorEdit4("Color", (float *)&color);
    ImGui::PopID();
    return recolor;
}
void Plotter::imSettings(){
  static int numRuns = 0;
  if(numRuns++ == 1){
    redraw = true;
    recolor = true;
    return;
  }

  redraw = ImGui::Button("redraw");
  ImGui::SameLine();
  redraw |= ImGui::InputInt("Seed", &seed);
  redraw |= ImGui::ListBox("Render Mode\n(single select)", &listbox_item_current, listbox_items, IM_ARRAYSIZE(listbox_items), 5);

  redraw |= ImGui::InputInt("NumLayers", &numLayers, 1, 10);
  layers.resize(numLayers);

  redraw |= ImGui::SliderFloat("rMin", &rmin, 0.0001f, rmax/2);
  redraw |= ImGui::SliderFloat("rMax", &rmax, rmin, 1.f);
  redraw |= ImGui::SliderInt("samples", &samples, 20, 5000);
  redraw |= ImGui::SliderFloat("frequency", &overlap, 0, 30);
  redraw |= ImGui::SliderFloat("SizeGamma", &gamma, 0, 20);

  recolor = false;
  recolor |= ImGui::SliderFloat("thickness", &thickness, 0, .05f);
  recolor |= ImGui::SliderFloat("showThreshhold", &showThreshhold, 0, 1.5);
  recolor |= ImGui::Checkbox("Use Image Colors", &useImageColors);
  for(size_t i=0; i < layers.size(); i++) {
      auto layer = layers[i];
      recolor |= layers[i].imSettings(i);
  }

  if(pixels == nullptr){
  	pixels = stbi_load("oni.jpg", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
  }
}

void Plotter::update(Processing * ctx){
  imSettings();

  if(redraw){
    int jiggle = 0;
    for(size_t i=0; i < layers.size(); i++) {
      auto& layer = layers[i];

      auto radiusFN = [=](const vec2 pos) {
        float metric;
        switch(listbox_item_current){
          case 0:
            metric = Random::range(0.0f, 1.0f);
            break;
          case 1:
            metric = length(pos);
            break;
          case 2:
            metric = abs(cos(pos.x * overlap) + sin(pos.y * overlap))/2;
            break;
          case 3:
            metric =1 - distance((vec3{SPLAT3(layer.color)}), (sampleImage(pos)));
            break;
          case 4:
            metric = abs(dot(pos, vec2{1,0}));
            break;
        }
        metric = std::min(std::max(metric, 0.01f), 0.99f);
        //metric = pow(metric, gamma);
        float sz = mix(rmin, rmax, metric);
        return vec2{sz * .5, sz};
      };
      Random::seed(seed + i*10 + jiggle);
      vector<Blob>& blobs = layer.blobs;
      blobs.clear();
      Partition p({-.8, -.8}, {1.6f, 1.6f}, rmax * 2);
      
      p.gen_random({-.8, -.8}, {.8, .8}, radiusFN, samples, blobs);
///      p.gen_poisson({-.8, -.8}, {.8, .8}, radiusFN, samples, blobs, 0.0001f);
      if(blobs.size() == 1 && jiggle < 5){
        i--;
        jiggle ++;
      }
    }
  }

  if(redraw || recolor){   
    ctx->clear();
     for(int i=0; i < numLayers; i++) {
      for(auto b: layers[i].blobs){
        if(b.r >= rmax * showThreshhold){
          if(useImageColors){
            b.render(ctx, vec4{sampleImage(b.pos),1.0},thickness);
          }
          else{
            b.render(ctx, layers[i].color, thickness);
          }
        }
      }
     }
    ctx->flush();
  }

  if(ImGui::Button("Save")){
    ProcessingSVG svg;
     for(int i=0; i < numLayers; i++) {
      svg.setFilename(Util::timestamp(i) );
      for(const auto& b: layers[i].blobs){
        if(b.r >= rmax * showThreshhold){
          svg.circle(b.pos, b.r);
        }
      }
      svg.render();
      svg.clear();

     }
  }

}