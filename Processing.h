#ifndef PROCESSING_H
#define PROCESSING_H

#include "Definitions.h"
#include <vector>

struct Buffer{
  uint handle =0;
  uint size =0;
  uint lastUsed=0;
};

class Processing{
	public:
    Processing();
    ~Processing();

    void flush();
		void clear();
		void push();
		void pop();
		void polygon(std::vector<vec2> v, bool loop = true);
		void line(vec2 p1, vec2 p2);

		void render();
	void dump();

	private:
  void allocate_buffers(unsigned int vbo_size, unsigned int ebo_size);

 	  mat4 view;
    std::vector<vec2> m_verts;
    std::vector<unsigned int> m_indices;
  int indexVert(vec2 p);


  Buffer m_VBO, m_EBO;
  uint m_VAO;
  //unsigned int m_EBO;
  //unsigned int m_vbo_size, m_ebo_size;
  //unsigned int m_vbo_ptr, m_ebo_ptr;
};


#endif // PROCESSING_H