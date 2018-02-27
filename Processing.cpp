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

void Processing::render() {
// convert to gl buffer
  glBindVertexArray(m_VAO);
  glDrawElements(GL_LINES, m_indices.size(), GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

int Processing::indexVert(vec2 p) {
  unsigned int idx = m_verts.size();
  m_verts.push_back(p);
  m_indices.push_back(idx);
  return idx;
}

void Processing::clear() {
  m_verts.clear();
  m_indices.clear();

}

void Processing::flush() {
  if((unsigned int)m_verts.size() > m_vbo_size){
    allocate_buffers((unsigned int)m_verts.size());
  }

  // TODO: only update data since last flush
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * ((unsigned int) m_verts.size()), m_verts.data() );
  glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(int) * ((unsigned int) m_indices.size()), m_indices.data() );

}

Processing::~Processing() {
  glDeleteVertexArrays(1, &m_VAO);
  glDeleteBuffers(1, &m_VBO);
  glDeleteBuffers(1, &m_EBO);
}

Processing::Processing() {
  allocate_buffers(2048);
}

void Processing::allocate_buffers(unsigned int min_size) {

  glGenVertexArrays(1, &m_VAO);
  glGenBuffers(1, &m_VBO);
  glGenBuffers(1, &m_EBO);

  glBindVertexArray(m_VAO);

  glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * std::max(min_size, (unsigned int) m_verts.size()), nullptr, GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * std::max(min_size, (unsigned int) m_indices.size()), nullptr, GL_DYNAMIC_DRAW);

  //TODO: Meshes/vert shaders should own their own attributes and bindings.
  // position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  // color attribute
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

}

