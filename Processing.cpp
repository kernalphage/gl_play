//
// Created by matt on 2/24/18.
//

#include "Processing.h"
#include "glm/ext.hpp"
#include <iostream>
#include <mapbox/earcut.hpp>

using namespace std;
void Processing::tri(UI_Vertex a, UI_Vertex b, UI_Vertex c)
{
	indexVert(a);
	indexVert(b);
	indexVert(c);
}
void Processing::polygon(std::vector<UI_Vertex> v, bool loop) {
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

void Processing::line(UI_Vertex p1, UI_Vertex p2) {
  indexVert(p1);
  indexVert(p2);
}
void Processing::dump(){
  for(auto& v : m_verts){
    cout<<to_string(v.pos)<<", ";
  }
  cout << endl;
  for(auto i : m_indices){
    cout<<i<<", ";
  }
  cout<<endl;
}

void Processing::render() {
// convert to gl buffer
  glBindVertexArray(m_VAO);
  glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

int Processing::indexVert(UI_Vertex p) {
  unsigned int idx = m_verts.size();
  m_verts.push_back(p);
  m_indices.push_back(idx);
  return idx;
}

void Processing::clear() {
  m_verts.clear();
  m_indices.clear();
  m_VBO.lastUsed = 0;
  m_EBO.lastUsed = 0;
}

void Processing::flush() {

  if(m_verts.size() > m_VBO.size || m_indices.size() > m_VBO.size){
    allocate_buffers(m_verts.size() * 2.3f, m_indices.size() * 2.3f);
  }
	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO.handle);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO.handle);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(UI_Vertex) * (m_verts.size()), m_verts.data());
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(int) * (m_indices.size()), m_indices.data());
  
  m_VBO.lastUsed = (uint) m_verts.size();
  m_EBO.lastUsed = (uint) m_indices.size();
}

Processing::~Processing() {
  glDeleteVertexArrays(1, &m_VAO);
  glDeleteBuffers(1, &m_VBO.handle);
  glDeleteBuffers(1, &m_EBO.handle);
}

Processing::Processing() {

  glGenVertexArrays(1, &m_VAO);
  glBindVertexArray(m_VAO);

  allocate_buffers(2, 2);
}

void Processing::allocate_buffers(unsigned int vbo_size, unsigned int ebo_size ) {

  if(m_VBO.handle){
    glDeleteBuffers(1, &m_VBO.handle);
    glDeleteBuffers(1, &m_EBO.handle);
  }
  cout<<"Allocating buffers: verts: " << m_VBO.size <<" ==> " << m_verts.size() <<" indices " << m_EBO.size<< " ==> "<<m_indices.size() <<  endl;

  m_VBO.size = std::max(vbo_size, (unsigned int) m_verts.size());
  m_EBO.size = std::max(ebo_size, (unsigned int) m_indices.size());

  glBindVertexArray(m_VAO);
  glGenBuffers(1, &m_VBO.handle);
  glGenBuffers(1, &m_EBO.handle);

  glBindBuffer(GL_ARRAY_BUFFER, m_VBO.handle);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO.handle);
  glBufferData(GL_ARRAY_BUFFER,          sizeof(UI_Vertex) * m_VBO.size,  nullptr, GL_DYNAMIC_DRAW);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,  sizeof(int)   * m_EBO.size, nullptr, GL_DYNAMIC_DRAW);
  //TODO: Meshes/vert shaders should own their own attributes and bindings.
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(UI_Vertex), (void*) 0 );
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(UI_Vertex), (void*)(sizeof(vec3)));
  glEnableVertexAttribArray(1);
}

