//
// Created by matt on 11/25/18.
//

#ifndef GL_PLAY_CELLULAR_HPP
#define GL_PLAY_CELLULAR_HPP
#include "../Definitions.hpp"
#include <vector>
#include <tuple>

class Cellular {
public:
  Cellular();
  // init();
  void imSettings(bool& redraw, bool&clear);
  void render(Processing* ctx, bool& redraw, bool& clear, int curFrame, int maxFrames);
  void settle();
  void add_edge(vec3 pos, vec3 dir);
private:
  struct CellNode{
    vec3 pos;
    bool outside=false;
    vec4 color{1,0,0,1};
  };

  void genPts();

  std::vector<CellNode> pts;
  std::vector<std::pair<int,int>> edges;

  int m_numpts = 3;
  float m_settleAmount = .000;
  float m_removeChance = 1.9;
  float m_edgeChance = .0099;
  int m_seed;
};


#endif //GL_PLAY_CELLULAR_HPP
