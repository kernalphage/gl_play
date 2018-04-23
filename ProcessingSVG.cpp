#include "Processing.hpp"
#include "image.capnp.h"
#include <capnp/message.h>
#include <capnp/serialize-packed.h>
#include <ctime>
#include <fcntl.h>
#include <fstream>


  void ProcessingSVG::tri(UI_Vertex a, UI_Vertex b, UI_Vertex c) {

  };
  void ProcessingSVG::quad(UI_Vertex a, UI_Vertex b, UI_Vertex c, UI_Vertex d) {

  };
  void ProcessingSVG::polygon(std::vector<UI_Vertex> v, bool loop) {

  };
  void ProcessingSVG::line(vec3 p1, vec3 p2, vec4 color) {
    m_lines.push_back(p1);
    m_lines.push_back(p2);
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

  capnp::List<Image::Command>::Builder commands = img.initCommands(m_pts.size() + m_lines.size());
  int cmd = 0;
  for(int i=0; i < m_pts.size(); i++){

    auto curCommand = commands[cmd++];
    curCommand.getCmd().setType(Image::Command::PrimitiveType::CIRCLE);
    curCommand.setValues( { m_pts[i].pos.x, m_pts[i].pos.y, m_pts[i].r } );
  };  
  for(int i=0; i < m_lines.size(); i+=2){
    commands[cmd].getCmd().setType(Image::Command::PrimitiveType::LINE);
    auto v = commands[cmd++].initValues(4);
    v.set(0, m_lines[i].x);
    v.set(1, m_lines[i].y);
    v.set(2, m_lines[i+1].x);
    v.set(3, m_lines[i+1].y);
  };

  int fd = open(m_filename.c_str(),O_RDWR );
 writePackedMessageToFd(fd, message); 
}

void ProcessingSVG::circle(vec2 pos, float radius) {
  m_pts.emplace_back(pos, radius);
};