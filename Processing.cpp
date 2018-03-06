//
// Created by matt on 2/24/18.
//

#include "Processing.h"
#include <iostream>
using namespace std;
void Processing::polygon(std::vector<vec2> v, bool loop) {
  int prev= indexVert(v[0]);
  int first = prev;
  for(int i=1; i < v.size(); i++){
    if(i!= 1){
     m_indices.push_back(prev);
    }  
    int cur = indexVert(v[i]);
    prev = cur;
  }
  if(loop){
    m_indices.push_back(prev);
    m_indices.push_back(first);
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

  if((unsigned int)m_verts.size() > m_VBO.size || (unsigned int)m_indices.size() > m_VBO.size){
    cout<<"Blew the buffer size " << endl;
    allocate_buffers((unsigned int)m_verts.size() * 1.5, (unsigned int)m_indices.size() * 1.5);
  }

  glBindVertexArray(m_VAO);
  glBindBuffer(GL_ARRAY_BUFFER, m_VBO.handle);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO.handle);

  glBufferSubData(GL_ARRAY_BUFFER,        sizeof(float) * m_VBO.lastUsed, sizeof(float) * (m_verts.size()   - m_VBO.lastUsed), m_verts.data()  + m_VBO.lastUsed );
  glBufferSubData(GL_ELEMENT_ARRAY_BUFFER,sizeof(int)   * m_EBO.lastUsed, sizeof(int)   * (m_indices.size() - m_EBO.lastUsed), m_indices.data()+ m_EBO.lastUsed );

  m_VBO.lastUsed = m_verts.size();   
  m_EBO.lastUsed = m_indices.size(); 
}

Processing::~Processing() {
  glDeleteVertexArrays(1, &m_VAO);
  glDeleteBuffers(1, &m_VBO.handle);
  glDeleteBuffers(1, &m_EBO.handle);
}

Processing::Processing() {

  glGenVertexArrays(1, &m_VAO);
  glBindVertexArray(m_VAO);

  allocate_buffers(100, 100);
}

void Processing::allocate_buffers(unsigned int vbo_size, unsigned int ebo_size ) {

  if(m_VBO.handle){
    glDeleteBuffers(1, &m_VBO.handle);
    glDeleteBuffers(1, &m_EBO.handle);
  }

  m_VBO.size = std::max(vbo_size, (unsigned int) m_verts.size());
  m_EBO.size = std::max(ebo_size, (unsigned int) m_indices.size());

  glBindVertexArray(m_VAO);
  glGenBuffers(1, &m_VBO.handle);
  glGenBuffers(1, &m_EBO.handle);

  glBindBuffer(GL_ARRAY_BUFFER, m_VBO.handle);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO.handle);
  glBufferData(GL_ARRAY_BUFFER,         sizeof(float) * m_VBO.size, m_verts.data(), GL_DYNAMIC_DRAW);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * m_EBO.size,   m_indices.data(), GL_DYNAMIC_DRAW);
  //TODO: Meshes/vert shaders should own their own attributes and bindings.
  // position attribute
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
}

