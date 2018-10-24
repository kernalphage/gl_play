#ifndef PROCESSING_HPP
#define PROCESSING_HPP

#include "Definitions.hpp"
#include <algorithm>
#include <functional>
#include <numeric>
#include <vector>
#include <string>

template<typename Vertex_Type, typename Extra_Data>
class Processing_t {
public:

  virtual void tri(Vertex_Type a, Vertex_Type b, Vertex_Type c)= 0;
  virtual void quad(Vertex_Type a, Vertex_Type b, Vertex_Type c, Vertex_Type d)= 0;
  virtual void polygon(std::vector<Vertex_Type> v, bool loop = true)= 0;
  virtual void spline(std::vector<vec3> points, Extra_Data middleColor, Extra_Data edgeColor, float thickness ) {
    for(int i =0; i < points.size() - 1; i++){
      line(points[i], points[i+1], middleColor);
    }
  }
  virtual void ngon(vec2 pos, float radius, int sides, Extra_Data innerColor, Extra_Data outerColor) = 0;

  virtual void line(vec3 p1, vec3 p2, Extra_Data color) = 0;   
  virtual void flush() = 0;
  virtual void clear() = 0;
  virtual void render() = 0;
};

struct Buffer {
  GLuint handle = 0;
  GLuint size = 0;
  GLuint lastUsed = 0;
};


template<typename Vertex_Type, typename Extra_Data>
class ProcessingGL_t : public Processing_t<Vertex_Type, Extra_Data> {
public: 
  ProcessingGL_t();
  ~ProcessingGL_t();
  void tri(Vertex_Type a, Vertex_Type b, Vertex_Type c) override;
  void quad(Vertex_Type a, Vertex_Type b, Vertex_Type c, Vertex_Type d) override;
  void polygon(std::vector<Vertex_Type> v, bool loop = true) override;
  void spline(std::vector<vec3> pts, Extra_Data middleColor, Extra_Data edgeColor, float thickness = .03) override ;
  void line(vec3 p1, vec3 p2, Extra_Data color) override;

  void ngon(vec2 pos, float radius, int sides, Extra_Data innerColor, Extra_Data outerColor) override;

  void flush() override;
  void clear() override;
  void render() override;
  void setMode(GLenum mode){ m_mode = mode;}

  int indexVert(Vertex_Type p);

private:
  void define_vertex_attributes();
  void allocate_buffers(unsigned int vbo_size, unsigned int ebo_size);
  GLenum m_mode;
  mat4 view;
  std::vector<Vertex_Type> m_verts;
  std::vector<unsigned int> m_indices;

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

template <typename Vertex_Type, typename Extra_Data>
class ProcessingSVG_t : public Processing_t<Vertex_Type, Extra_Data> {
public:

  void setFilename(std::string filename) { m_filename = filename;};
  void setStrokeColor(vec3 color){m_color =color;};
  void circle(vec2 pos, float radius);

  void tri(Vertex_Type a, Vertex_Type b, Vertex_Type c) override;
  void quad(Vertex_Type a, Vertex_Type b, Vertex_Type c, Vertex_Type d) override;
  void polygon(std::vector<Vertex_Type> v, bool loop = true) override;
  void line(vec3 p1, vec3 p2, Extra_Data color) override;

  void ngon(vec2 pos, float radius, int sides, Extra_Data innerColor, Extra_Data outerColor) override {
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

using Processing = Processing_t<UI_Vertex, vec4>;
using ProcessingGL = ProcessingGL_t<UI_Vertex, vec4>;
using ProcessingSVG = ProcessingSVG_t<UI_Vertex, vec4>;

#endif // PROCESSING_HPP
