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

  glDrawElements(m_mode, m_indices.size(), GL_UNSIGNED_INT, 0);
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

void ProcessingGL::spline(vector<vec3> pts, vec4 middleColor, vec4 edgeColor, float thickness ) {

  if(pts.size() <= 3){
    //line(pts[0], pts[1], middleColor); // todo: make line take the same parameters
    return;
  }
  auto perp = [](vec3 a, vec3 b){ vec3 d = normalize(a-b); return  vec3(d.y, -d.x, (a.z+b.z)*.5);};

  for(int i=0; i < pts.size() - 3; i++){
    auto prevPerp = perp(pts[i+0], pts[i+1])* thickness;
    vec3 curPerp =  perp(pts[i+1], pts[i+2])* thickness;
    auto nextPerp = perp(pts[i+2], pts[i+3])* thickness;

    vec3 startPerp = (curPerp + prevPerp) *.5;
    vec3 endPerp =  (curPerp + nextPerp) * .5;
    quad(UI_Vertex{pts[i], middleColor},
         UI_Vertex{pts[i]  -startPerp, edgeColor},
         UI_Vertex{pts[i+1]-endPerp, edgeColor},
         UI_Vertex{pts[i+1], middleColor});

    quad(UI_Vertex{pts[i]+startPerp, edgeColor},
         UI_Vertex{pts[i], middleColor},
         UI_Vertex{pts[i+1], middleColor},
         UI_Vertex{pts[i+1]+endPerp, edgeColor});
  }

}

void ProcessingGL::ngon(vec2 pos, float r, int sides, vec4 outerColor, vec4 innerColor) {
  const float pi = 3.1415f;
  vec3 threepos{pos, 1};
  float dTheta = (2 * pi) / sides;
  auto cPos = [=](float t, float radius){ return vec3{sin(t), cos(t), 1} * radius + threepos;};
  for (float theta = 0; theta <= (2 * pi); theta += dTheta) {
    tri(UI_Vertex{cPos(theta, r), outerColor},
              UI_Vertex{cPos(theta + dTheta, r), outerColor},
              UI_Vertex{cPos(theta + dTheta, 0), innerColor});
  }
}
