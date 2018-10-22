#include "Processing.hpp"
#include <iostream>
#include <fstream>
using namespace std;

template<typename Vertex_Type, typename Extra_Data>
  void ProcessingSVG_t<Vertex_Type, Extra_Data>::tri(Vertex_Type a, Vertex_Type b, Vertex_Type c) {

  };
template<typename Vertex_Type, typename Extra_Data>
  void ProcessingSVG_t<Vertex_Type, Extra_Data>::quad(Vertex_Type a, Vertex_Type b, Vertex_Type c, Vertex_Type d) {

  };
template<typename Vertex_Type, typename Extra_Data>
  void ProcessingSVG_t<Vertex_Type, Extra_Data>::polygon(std::vector<Vertex_Type> v, bool loop) {

  };
template<typename Vertex_Type, typename Extra_Data>
  void ProcessingSVG_t<Vertex_Type, Extra_Data>::line(vec3 p1, vec3 p2, Extra_Data color) {
    m_lines.push_back(p1);
    m_lines.push_back(p2);
  };
template<typename Vertex_Type, typename Extra_Data>
  void ProcessingSVG_t<Vertex_Type, Extra_Data>::flush() {

  };
template<typename Vertex_Type, typename Extra_Data>
  void ProcessingSVG_t<Vertex_Type, Extra_Data>::clear() {
    m_pts.clear();
  };
template<typename Vertex_Type, typename Extra_Data>
  void ProcessingSVG_t<Vertex_Type, Extra_Data>::render(){
    std::ofstream file;
    file.open(m_filename, ios::out);

    file <<"stroke "<<SPLAT_OP_3(m_color, << " " <<) << "\n";

    //file <<"stroke "<<m_color.x <<" "<<m_color.y<<" " <<m_color.z<<"\n";

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

template<typename Vertex_Type, typename Extra_Data>
void ProcessingSVG_t<Vertex_Type, Extra_Data>::circle(vec2 pos, float radius) {
  m_pts.emplace_back(pos, radius);
};

template class ProcessingSVG_t<UI_Vertex, vec4>;