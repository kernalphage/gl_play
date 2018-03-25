#ifndef PROCESSING_H
#define PROCESSING_H

#include "Definitions.h"
#include <algorithm>
#include <functional>
#include <vector>
#include <numeric>   

struct Buffer{
  GLuint handle =0;
  uint size =0;
  uint lastUsed=0;
};

class Processing{
	public:
    Processing();
    ~Processing();

    void flush();
		void clear();
		void push(mat4 view){
      cur *= view;
      viewStack.push_back(view);
    }
		void pop(){
      viewStack.pop_back();
      cur = std::accumulate(viewStack.begin(), viewStack.end(),glm::mat4(1.0), std::multiplies<mat4>() );
    }

		void tri(UI_Vertex a, UI_Vertex b, UI_Vertex c);
		void polygon(std::vector<UI_Vertex> v, bool loop = true);
		void line(UI_Vertex p1, UI_Vertex p2);

		void render();
	void dump();

	private:
  void allocate_buffers(unsigned int vbo_size, unsigned int ebo_size);

 	  mat4 view;
    std::vector<UI_Vertex> m_verts;
    std::vector<unsigned int> m_indices;
  int indexVert(UI_Vertex p);

  mat4 cur;
  std::vector<mat4> viewStack;

  Buffer m_VBO, m_EBO;
  GLuint m_VAO;
};


#endif // PROCESSING_H