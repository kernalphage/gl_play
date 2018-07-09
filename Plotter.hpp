//
// Created by matt on 4/25/18.
//

#ifndef GL_PLAY_PLOTTER_HPP
#define GL_PLAY_PLOTTER_HPP
#include "Definitions.hpp"
#include "Blob.hpp"
#include "Random.hpp"
#include "imguifilesystem.h"
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
  void update(Processing* ctx, float t);
  void imSettings();

private:

  void matchImage();

vec3 sampleImage(const vec2 pos, int channel = 0) {
      vec2 v2 = (pos + vec2{1, 1}) / 2;
      int ex = (int) floor((v2.x) * texWidth);
      int wy = (int) floor((v2.y) * texHeight);
      ex = glm::min(glm::max(ex, 0), texWidth - 1);
      wy = glm::min(glm::max(texHeight - wy, 0), texHeight - 1);
      int idx = (ex + wy * texWidth) * 4;
      return vec3{pixels[idx + 0],pixels[idx + 1],pixels[idx + 2]} / 255;
};

  bool redraw = false;
  bool recolor = false;

  float rmin = .01;
  float rmax = .05;
  float showThreshhold = 1.90;
  int samples = 400;
  float overlap = 1.84f;
  float amplitude = 1.0;
  vec4 innerColor;
  int seed = 0;
  int numLayers = 4;
  std::vector<LayerSettings> layers = {
//  {{},{1.0,0.0,0.0, 1.0}}, 
//  {{},{0.0,1.0,0.0, 1.0}}, 
//  {{},{0.0,0.0,1.0, 1.0}}
      /*{{},{0.9019607843,	0.9215686275,	0.5607843137,1}},
      {{},{0.9176470588,	0.9607843137,	0.9294117647,1}},
      {{},{0.5921568627,	0.8117647059,	0.5647058824,1}},
      {{},{0.066666666,	0.5803921569,	0.3843137255,1}}
       */

{{}, {0.0, 0.0, 0.0, 1.0}},
{{}, {0.0, 0.0, 0.9411764705882353, 1.0}},
{{}, {0.0, 0.9411764705882353, 0.0, 1.0}},
{{}, {0.6470588235294118, 0.00392156862745098, 0.23921568627450981, 1.0}},
{{}, {0.8196078431372549, 0.49019607843137253, 0.807843137254902, 1.0}},
{{}, {0.40784313725490196, 0.28627450980392155, 0.10196078431372549, 1.0}},
{{}, {0.027450980392156862, 0.5843137254901961, 0.7372549019607844, 1.0}}, 
{{}, {0.3764705882352941, 1.0, 0.10980392156862745, 1.0}},
{{}, {1.0, 0.0, 0.0, 1.0}}, 
{{}, {1.0, 0.3058823529411765, 0.7607843137254902, 1.0}},
{{}, {1.0, 0.5490196078431373, 0.0, 1.0}}

};
  float gamma = 2;
  float thickness = .01;
  int listbox_item_current = 1;
  bool useImageColors = false;


int texWidth, texHeight, texChannels;
stbi_uc* pixels = nullptr;
static ImGuiFs::Dialog dlg;  // one per dialog (and must be static)

};


#endif //GL_PLAY_PLOTTER_HPP
