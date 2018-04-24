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
    m_pts.clear();
  };
  void ProcessingSVG::render(){

    std::ofstream file;
    file.open(m_filename, ios::out);
    vec2 sz_half = m_pagesize * .5f;
    float maxSz = std::min(sz_half.x, sz_half.y) ; // Scale to parent (assuming [-1, 1])
    for(int i=0; i < m_pts.size(); i++){
      file<<"c"
          << " "<< (m_pts[i].pos.x * maxSz + sz_half.x) * ppi
          << " "<< (m_pts[i].pos.y * maxSz + sz_half.y) * ppi
          << " "<< m_pts[i].r * ppi << "\n";
    };

    file.flush();
    file.close();
    string cmd = "python svgbuilder.py ";
    cmd += m_filename;
    system(cmd.c_str());
}

void ProcessingSVG::circle(vec2 pos, float radius) {
  m_pts.emplace_back(pos, radius);
};