#ifndef PROCESSING_HPP
#define PROCESSING_HPP

#include "Definitions.hpp"
#include <algorithm>
#include <functional>
#include <numeric>
#include <vector>

struct Buffer {
  GLuint handle = 0;
  uint size = 0;
  uint lastUsed = 0;
};

class Processing {
public:
  Processing();
  ~Processing();

  void flush();
  void clear();
  void pushT(mat4 view) {
    m_cur *= view;
    viewStack.push_back(view);
  }
  void popT() {
    viewStack.pop_back();
    m_cur = std::accumulate(viewStack.begin(), viewStack.end(), glm::mat4(1.0),
                          std::multiplies<mat4>());
  }

  void tri(UI_Vertex a, UI_Vertex b, UI_Vertex c);
  void quad(UI_Vertex a, UI_Vertex b, UI_Vertex c, UI_Vertex d);
  void polygon(std::vector<UI_Vertex> v, bool loop = true);
  void line(vec3 p1, vec3 p2, vec4 color);

  void render();
  void dump();

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

#endif // PROCESSING_HPP