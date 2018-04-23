#include "Processing.hpp"
#include "image.capnp.h"
#include <capnp/message.h>
#include <capnp/serialize-packed.h>
#include <ctime>
#include <fcntl.h>
#include <iostream>
#include <fstream>
using namespace std;

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

    std::ofstream file;
    file.open(m_filename, ios::out);

    for(int i=0; i < m_pts.size(); i++){
      file<<"c"
          << " "<< m_pts[i].pos.x
          << " "<< m_pts[i].pos.y
          << " "<< m_pts[i].r<< "\n";
    };

    file.flush();
    file.close();
}

void ProcessingSVG::circle(vec2 pos, float radius) {
  m_pts.emplace_back(pos, radius);
};