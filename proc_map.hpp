//
// Created by matt on 10/14/18.
//

#ifndef GL_PLAY_PROC_MAP_HPP
#define GL_PLAY_PROC_MAP_HPP
#include "Definitions.hpp"
#include <donerserializer/DonerSerialize.h>
#include <donerserializer/DonerDeserialize.h>
#include "Processing.hpp"

struct ColorWheel{
  vec4 a{0,0,0,1};
  vec4 b{0,0,0,1};
  bool imSettings(const char* str_id){
    ImGui::PushID(str_id);
    bool redraw = false;
    redraw |= ImGui::ColorEdit4("a", (float *)&a);
    redraw |= ImGui::ColorEdit4("b", (float *)&b);
    ImGui::PopID();
    return redraw;
  }
  vec4 sample(float t){
    float x = sin(t*6.28f);
    return lerp(a,b,x/2+.5f);

  };
};


class proc_map {

public:

  bool imSettings();
  void render(ProcessingGL* ctx);

  int m_chunksize = 10;
  int m_seed =0;
  float m_frequency = .5;
  float waterThreshhld = .3;
  ColorWheel g;
  ColorWheel w;
private:
  bool do_serialize();
};


DONER_DEFINE_REFLECTION_DATA(proc_map,
  DONER_ADD_NAMED_VAR_INFO( m_chunksize, "m_chunksize"),
  DONER_ADD_NAMED_VAR_INFO( m_seed, "m_seed" ),
  DONER_ADD_NAMED_VAR_INFO( m_frequency, "m_frequency"),
  DONER_ADD_NAMED_VAR_INFO( waterThreshhld, "waterThreshhld")
)

#endif //GL_PLAY_PROC_MAP_HPP
