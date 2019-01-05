//
// Created by matt on 2/24/18.
//

#include "Processing.hpp"
#include "glm/ext.hpp"
#include <iostream>

using namespace std;


template<typename Vertex_Type, typename Extra_Data>
ProcessingGL_t<Vertex_Type, Extra_Data>::ProcessingGL_t() {

  glGenVertexArrays(1, &m_VAO);
  glBindVertexArray(m_VAO);

  allocate_buffers(2, 2);
}

template<typename Vertex_Type, typename Extra_Data>
ProcessingGL_t<Vertex_Type, Extra_Data>::~ProcessingGL_t() {
  glDeleteVertexArrays(1, &m_VAO);
  glDeleteBuffers(1, &m_VBO.handle);
  glDeleteBuffers(1, &m_EBO.handle);
}

template<typename Vertex_Type, typename Extra_Data>
void ProcessingGL_t<Vertex_Type, Extra_Data>::polygon(std::vector<Vertex_Type> v, bool loop) {
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

template<typename Vertex_Type, typename Extra_Data>
void ProcessingGL_t<Vertex_Type, Extra_Data>::tri(Vertex_Type a, Vertex_Type b, Vertex_Type c) {
  indexVert(a);
  indexVert(b);
  indexVert(c);
}

template<typename Vertex_Type, typename Extra_Data>
void ProcessingGL_t<Vertex_Type, Extra_Data>::quad(Vertex_Type a, Vertex_Type b, Vertex_Type c, Vertex_Type d){
  tri(a,b,c);
  tri(a,c,d);
}

template<typename Vertex_Type, typename Extra_Data>
void ProcessingGL_t<Vertex_Type, Extra_Data>::line(vec3 p1, vec3 p2, Extra_Data color) {
  line(p1, p2, color, .02);
}

template<typename Vertex_Type, typename Extra_Data>
void ProcessingGL_t<Vertex_Type, Extra_Data>::line(vec3 p1, vec3 p2, Extra_Data color, float thickness) {
  auto perp = normalize(rotateZ((p1-p2), 3.14f/2)) * thickness;
  quad({p1 + perp, color},
       {p2 + perp, color},
       {p2 - perp, color},
       {p1 - perp, color});
}


template<typename Vertex_Type, typename Extra_Data>
void ProcessingGL_t<Vertex_Type, Extra_Data>::render() {
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

template<typename Vertex_Type, typename Extra_Data>
int ProcessingGL_t<Vertex_Type, Extra_Data>::indexVert(Vertex_Type p) {
  unsigned int idx = m_verts.size();
  vec4 pos = m_cur * vec4(p.pos, 1);
  p.pos = vec3(SPLAT3(pos));
  m_verts.push_back(p);
  m_indices.push_back(idx);
  return idx;
}

template<typename Vertex_Type, typename Extra_Data>
void ProcessingGL_t<Vertex_Type, Extra_Data>::clear() {
  m_verts.clear();
  m_indices.clear();
  m_VBO.lastUsed = 0;
  m_EBO.lastUsed = 0;
    viewStack.clear();
    m_cur = glm::mat4(1.0);
}

template<typename Vertex_Type, typename Extra_Data>
void ProcessingGL_t<Vertex_Type, Extra_Data>::flush() {

  if (m_verts.size() > m_VBO.size || m_indices.size() > m_VBO.size) {
    allocate_buffers(m_verts.size() * 2.3f, m_indices.size() * 2.3f);
  }
  glBindVertexArray(m_VAO);
  glBindBuffer(GL_ARRAY_BUFFER, m_VBO.handle);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO.handle);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex_Type) * (m_verts.size()),
                  m_verts.data());
  glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(int) * (m_indices.size()),
                  m_indices.data());

  m_VBO.lastUsed = (uint)m_verts.size();
  m_EBO.lastUsed = (uint)m_indices.size();
}



template<typename Vertex_Type, typename Extra_Data>
void ProcessingGL_t<Vertex_Type, Extra_Data>::allocate_buffers(unsigned int vbo_size,
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
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex_Type) * m_VBO.size, nullptr,
               GL_DYNAMIC_DRAW);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * m_EBO.size, nullptr,
               GL_DYNAMIC_DRAW);

  define_vertex_attributes();
}


template<typename Vertex_Type, typename Extra_Data>
void ProcessingGL_t<Vertex_Type, Extra_Data>::spline(vector<vec3> pts, Extra_Data middleColor, Extra_Data edgeColor, float thickness ) {

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
    quad(Vertex_Type{pts[i], middleColor},
         Vertex_Type{pts[i]  -startPerp, edgeColor},
         Vertex_Type{pts[i+1]-endPerp, edgeColor},
         Vertex_Type{pts[i+1], middleColor});

    quad(Vertex_Type{pts[i]+startPerp, edgeColor},
         Vertex_Type{pts[i], middleColor},
         Vertex_Type{pts[i+1], middleColor},
         Vertex_Type{pts[i+1]+endPerp, edgeColor});
  }

}

template<typename Vertex_Type, typename Extra_Data>
void ProcessingGL_t<Vertex_Type, Extra_Data>::ngon(vec2 pos, float r, int sides, Extra_Data outerColor, Extra_Data innerColor) {
  const float pi = 3.1415f;
  vec3 threepos{pos, 1};
  float dTheta = (2 * pi) / sides;
  auto cPos = [=](float t, float radius){ return vec3{sin(t), cos(t), 1} * radius + threepos;};
  for (float theta = 0; theta <= (2 * pi); theta += dTheta) {
    tri(Vertex_Type{cPos(theta, r), outerColor},
              Vertex_Type{cPos(theta + dTheta, r), outerColor},
              Vertex_Type{cPos(theta + dTheta, 0), innerColor});
  }
}

// To define a specialization of a vertex type, add a template class decleration and 
// provide an impmentation of define_vertex_attributes();

/// UI_Vertex 

template <>
void ProcessingGL_t<UI_Vertex, vec4>::define_vertex_attributes(){
  
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(UI_Vertex), (void *)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(UI_Vertex),
                        (void *)(sizeof(vec3)));
  glEnableVertexAttribArray(1);
}

template class ProcessingGL_t<UI_Vertex, vec4>;


// Textured particles
template <>
void ProcessingGL_t<Particle_Vertex, Particle_Vertex::particle_data>::define_vertex_attributes(){
  
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle_Vertex), (void *)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Particle_Vertex),
                        (void *)(sizeof(vec3)));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Particle_Vertex),
                        (void *)(sizeof(vec3) + sizeof(vec2)));

  glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Particle_Vertex),
                        (void *)(sizeof(vec3) + sizeof(vec2) + sizeof(float)));

  glEnableVertexAttribArray(3);
}
template class ProcessingGL_t<Particle_Vertex, Particle_Vertex::particle_data>;