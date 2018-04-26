//
// Created by matt on 4/25/18.
//

#ifndef GL_PLAY_PLOTTER_HPP
#define GL_PLAY_PLOTTER_HPP
#include "Definitions.hpp"
#include "Blob.hpp"
#include "Random.hpp"
#include <stb_image.h>
#include <vector>

struct LayerSettings
{
  vector<Blob> blobs;
  vec4 color;
  bool imSettings(int id);
};

class Processing;

class Plotter {
public:
  void update(Processing* ctx);
  void imSettings();

private:

vec3 sampleImage(const vec2 pos, int channel = 0) {
      vec2 v2 = (pos + vec2{1, 1}) / 2;
      int ex = (int) floor((v2.x) * texWidth);
      int wy = (int) floor((v2.y) * texHeight);
      ex = std::min(std::max(ex, 0), texWidth - 1);
      wy = std::min(std::max(texHeight - wy, 0), texHeight - 1);
      int idx = (ex + wy * texWidth) * 4;
      return vec3{pixels[idx + 0],pixels[idx + 1],pixels[idx + 2]} / 255;
};

  bool redraw = false;
  bool recolor = false;

  float rmin = .01;
  float rmax = .15;
  float showThreshhold = .10;
  int samples = 1400;
  float overlap = 4.84f;
  vec4 innerColor;
  int seed = 0;
  int numLayers = 3;
  std::vector<LayerSettings> layers = {{{},{1.0,0.0,0.0, 1.0}}, {{},{0.0,1.0,0.0, 1.0}}, {{},{0.0,0.0,1.0, 1.0}}};
  float gamma = 2;
  float thickness = .01;
  int listbox_item_current = 1;
  bool useImageColors = false;


int texWidth, texHeight, texChannels;
stbi_uc* pixels = nullptr;

};


#endif //GL_PLAY_PLOTTER_HPP
