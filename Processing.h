#ifndef PROCESSING_H
#define PROCESSING_H

#include "Definitions.h"
#include <vector>

class Processing{
	public:
    Processing();
    ~Processing();

    void flush();
		void clear();
		void push();
		void pop();
		void polygon(std::vector<vec2> v);
		void line(vec2 p1, vec2 p2);

		void render();
    
	private:
  void allocate_buffers(unsigned int min_size);

 	  mat4 view;
    std::vector<vec2> m_verts;
    std::vector<unsigned int> m_indices;
  int indexVert(vec2 p);

  unsigned int m_VBO, m_VAO;
  unsigned int m_EBO;
  unsigned int m_vbo_size, m_vao_size;

};


#endif // PROCESSING_H