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
  void line(vec3 p1, vec3 p2, vec4 color) override;
  
  void flush() override;
  void clear() override;
  void render() override;

private:

  void allocate_buffers(unsigned int vbo_size, unsigned int ebo_size);

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

  void setFilename(std::string filename);

  void circle(vec2 pos, float radius);

  void tri(UI_Vertex a, UI_Vertex b, UI_Vertex c) override;
  void quad(UI_Vertex a, UI_Vertex b, UI_Vertex c, UI_Vertex d) override;
  void polygon(std::vector<UI_Vertex> v, bool loop = true) override;
  void line(vec3 p1, vec3 p2, vec4 color) override;
  
  void flush() override;
  void clear() override;
  void render() override;

private:

  std::string m_filename;
  std::vector<circ> m_pts;
};

#endif // PROCESSING_HPP