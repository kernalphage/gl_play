//
// Created by matt on 2/24/18.
//

#include "Processing.hpp"
#include "glm/ext.hpp"
#include <iostream>

using namespace std;


ProcessingGL::ProcessingGL() {

  glGenVertexArrays(1, &m_VAO);
  glBindVertexArray(m_VAO);

  allocate_buffers(2, 2);
}
ProcessingGL::~ProcessingGL() {
  glDeleteVertexArrays(1, &m_VAO);
  glDeleteBuffers(1, &m_VBO.handle);
  glDeleteBuffers(1, &m_EBO.handle);
}

void ProcessingGL::polygon(std::vector<UI_Vertex> v, bool loop) {
  int prev = indexVert(v[0]);
  int first = prev;
  for (int i = 1; i < v.size(); i++) {
    if (i != 1) {
      m_indices.push_back(prev);
    }
    int cur = indexVert(v[i]);
    prev = cur;
  }
  if (loop) {
    m_indices.push_back(prev);
    m_indices.push_back(first);
  }
}

void ProcessingGL::tri(UI_Vertex a, UI_Vertex b, UI_Vertex c) {
  indexVert(a);
  indexVert(b);
  indexVert(c);
}
void ProcessingGL::quad(UI_Vertex a, UI_Vertex b, UI_Vertex c, UI_Vertex d){
  tri(a,b,c);
  tri(a,c,d);
}


void ProcessingGL::line(vec3 p1, vec3 p2, vec4 color) {
  auto perp = normalize(rotateZ((p1-p2), 3.14f/2)) * .02;
  quad({p1 + perp, color}, 
       {p2 + perp, color}, 
       {p2 - perp, color},
       {p1 - perp, color}); 
}

void ProcessingGL::render() {
  // convert to gl buffer

  glBindVertexArray(m_VAO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO.handle);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, m_VBO.handle);

    glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
}

int ProcessingGL::indexVert(UI_Vertex p) {
  unsigned int idx = m_verts.size();
  vec4 pos = m_cur* vec4(p.pos, 1);
  p.pos = vec3(SPLAT3(pos));
  m_verts.push_back(p);
  m_indices.push_back(idx);
  return idx;
}

void ProcessingGL::clear() {
  m_verts.clear();
  m_indices.clear();
  m_VBO.lastUsed = 0;
  m_EBO.lastUsed = 0;
    viewStack.clear();
    m_cur = glm::mat4(1.0);
}

void ProcessingGL::flush() {

  if (m_verts.size() > m_VBO.size || m_indices.size() > m_VBO.size) {
    allocate_buffers(m_verts.size() * 2.3f, m_indices.size() * 2.3f);
  }
  glBindVertexArray(m_VAO);
  glBindBuffer(GL_ARRAY_BUFFER, m_VBO.handle);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO.handle);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(UI_Vertex) * (m_verts.size()),
                  m_verts.data());
  glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(int) * (m_indices.size()),
                  m_indices.data());

  m_VBO.lastUsed = (uint)m_verts.size();
  m_EBO.lastUsed = (uint)m_indices.size();
}

void ProcessingGL::allocate_buffers(unsigned int vbo_size,
                                  unsigned int ebo_size) {

  if (m_VBO.handle) {
    glDeleteBuffers(1, &m_VBO.handle);
    glDeleteBuffers(1, &m_EBO.handle);
  }
  cout << "Allocating buffers: verts: " << m_VBO.size << " ==> "
       << m_verts.size() << " indices " << m_EBO.size << " ==> "
       << m_indices.size() << endl;

  m_VBO.size = std::max(vbo_size, (unsigned int)m_verts.size());
  m_EBO.size = std::max(ebo_size, (unsigned int)m_indices.size());

  glBindVertexArray(m_VAO);
  glGenBuffers(1, &m_VBO.handle);
  glGenBuffers(1, &m_EBO.handle);

  glBindBuffer(GL_ARRAY_BUFFER, m_VBO.handle);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO.handle);
  glBufferData(GL_ARRAY_BUFFER, sizeof(UI_Vertex) * m_VBO.size, nullptr,
               GL_DYNAMIC_DRAW);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * m_EBO.size, nullptr,
               GL_DYNAMIC_DRAW);
  // TODO: Meshes/vert shaders should own their own attributes and bindings.
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(UI_Vertex), (void *)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(UI_Vertex),
                        (void *)(sizeof(vec3)));
  glEnableVertexAttribArray(1);
}

void ProcessingGL::spline(vector<vec3> pts, vec4 color, float thickness ) {
 auto prevPerp = normalize(rotateZ((pts[0], pts[1]), 3.14f/2)) * thickness;
  for(int i=0; i < pts.size() - 1; i++){
    vec3 curPerp = prevPerp;
    vec3 dPos = (pts[i] - pts[i+1]);
    if(length(dPos) > thickness * .0001f){
      curPerp = normalize(rotateZ(dPos, 3.14f/2)) * thickness;
    }
    vec3 p = pts[i];
    quad(UI_Vertex{pts[i]+prevPerp, color},
         UI_Vertex{pts[i]-prevPerp, color},
         UI_Vertex{pts[i+1]-curPerp, color},
         UI_Vertex{pts[i+1]+curPerp, color});
     prevPerp = curPerp;

  }

}
