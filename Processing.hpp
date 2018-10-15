#ifndef PROCESSING_HPP
#define PROCESSING_HPP

#include "Definitions.hpp"
#include <algorithm>
#include <functional>
#include <numeric>
#include <vector>
#include <string>


class Processing {
public:

  virtual void tri(UI_Vertex a, UI_Vertex b, UI_Vertex c)= 0;
  virtual void quad(UI_Vertex a, UI_Vertex b, UI_Vertex c, UI_Vertex d)= 0;
  virtual void polygon(std::vector<UI_Vertex> v, bool loop = true)= 0;
  virtual void spline(std::vector<vec3> points, vec4 middleColor, vec4 edgeColor, float thickness ) {
    for(int i =0; i < points.size() - 1; i++){
      line(points[i], points[i+1], middleColor);
    }
  }
  virtual void ngon(vec2 pos, float radius, int sides, vec4 innerColor, vec4 outerColor) = 0;

  virtual void line(vec3 p1, vec3 p2, vec4 color) = 0;   
  virtual void flush() = 0;
  virtual void clear() = 0;
  virtual void render() = 0;
};

struct Buffer {
  GLuint handle = 0;
  uint size = 0;
  uint lastUsed = 0;
};

class ProcessingGL : public Processing {
public: 
  ProcessingGL();
  ~ProcessingGL();
  void tri(UI_Vertex a, UI_Vertex b, UI_Vertex c) override;
  void quad(UI_Vertex a, UI_Vertex b, UI_Vertex c, UI_Vertex d) override;
  void polygon(std::vector<UI_Vertex> v, bool loop = true) override;
  void spline(std::vector<vec3> pts, vec4 middleColor, vec4 edgeColor, float thickness = .03) override ;
  void line(vec3 p1, vec3 p2, vec4 color) override;

  void ngon(vec2 pos, float radius, int sides, vec4 innerColor, vec4 outerColor) override;

  void flush() override;
  void clear() override;
  void render() override;
  void setMode(GLenum mode){ m_mode = mode;}

private:

  void allocate_buffers(unsigned int vbo_size, unsigned int ebo_size);
  GLenum m_mode;
  mat4 view;
  std::vector<UI_Vertex> m_verts;
  std::vector<unsigned int> m_indices;
  int indexVert(UI_Vertex p);

  mat4 m_cur;
  std::vector<mat4> viewStack;

  Buffer m_VBO, m_EBO;
  GLuint m_VAO;
};

struct circ{
  circ(vec2 p, float rad): pos(p), r(rad){};
  vec2 pos;
  float r;
};

class ProcessingSVG : public Processing {
public:

  void setFilename(std::string filename) { m_filename = filename;};
  void setStrokeColor(vec3 color){m_color =color;};
  void circle(vec2 pos, float radius);

  void tri(UI_Vertex a, UI_Vertex b, UI_Vertex c) override;
  void quad(UI_Vertex a, UI_Vertex b, UI_Vertex c, UI_Vertex d) override;
  void polygon(std::vector<UI_Vertex> v, bool loop = true) override;
  void line(vec3 p1, vec3 p2, vec4 color) override;

  void ngon(vec2 pos, float radius, int sides, vec4 innerColor, vec4 outerColor) {
    circle(pos, radius);
  };


  void flush() override;
  void clear() override;
  void render() override;

private:

  vec2 m_pagesize{8.5,11};
  float ppi = 300;
  vec3 m_color{0,0,0};
  std::string m_filename;
  std::vector<circ> m_pts;
  std::vector<vec3> m_lines;
};

#endif // PROCESSING_HPP