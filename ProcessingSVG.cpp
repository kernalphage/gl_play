#include "Processing.hpp"
#include "image.capnp.h"
#include <capnp/message.h>
#include <capnp/serialize-packed.h>
#include <ctime>
#include <fcntl.h>


  void ProcessingSVG::tri(UI_Vertex a, UI_Vertex b, UI_Vertex c) {

  };
  void ProcessingSVG::quad(UI_Vertex a, UI_Vertex b, UI_Vertex c, UI_Vertex d) {

  };
  void ProcessingSVG::polygon(std::vector<UI_Vertex> v, bool loop) {

  };
  void ProcessingSVG::line(vec3 p1, vec3 p2, vec4 color) {

  };
  void ProcessingSVG::flush() {

  };
  void ProcessingSVG::clear() {

  };
  void ProcessingSVG::render(){
  	 ::capnp::MallocMessageBuilder message;
  Image::Builder img = message.initRoot<Image>();
  img.setId( 0 );
  img.setShortname(m_filename.c_str());
  img.setTimestamp(time(nullptr));

  capnp::List<Image::Command>::Builder commands = img.initCommands(m_pts.size());

  for(int i=0; i < m_pts.size(); i++){
    commands[i].getCmd().setType(Image::Command::PrimitiveType::CIRCLE);

    auto v = commands[i].initValues(3);
    v.set(0, m_pts[i].pos.x);
    v.set(1, m_pts[i].pos.y);
    v.set(2, m_pts[i].r);
  };
}

void ProcessingSVG::circle(vec2 pos, float radius) {
  m_pts.emplace_back(pos, radius);

};