//
// Created by matt on 2/24/18.
//

#include "Processing.h"

void Processing::polygon(std::vector<vec2> v) {
  int prev= indexVert(v[0]);

  for(int i=1; i < v.size(); i++){
    if(i!= 1) m_indices.push_back(prev);
    int cur = indexVert(v[i]);
    prev = cur;
  }
}

void Processing::line(vec2 p1, vec2 p2) {
  indexVert(p1);
  indexVert(p2);
}

void Processing::render(std::vector<float> &ctx) {
// convert to gl buffer

}

int Processing::indexVert(vec2 p) {
  int idx = m_verts.size();
  m_verts.push_back(p);
  m_indices.push_back(idx);
  return idx;
}

void Processing::clear() {


}

void Processing::flush() {

}

Processing::~Processing() {
  glDeleteBuffers(1, &m_VBO);
  glDeleteBuffers(1, &m_VAO);
}

Processing::Processing() {
  allocate_buffers(2048);
}

void Processing::allocate_buffers(int max_size) {

  glGenVertexArrays(1, &m_VAO);
  glGenBuffers(1, &m_VBO);

  glBindVertexArray(m_VAO);

  glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * max_size, nullptr, GL_STATIC_DRAW);

  //TODO: Meshes/vert shaders should own their own attributes and bindings.
  // position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  // color attribute
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

}

