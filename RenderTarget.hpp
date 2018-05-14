//
// Created by matt on 4/28/18.
//

#ifndef GL_PLAY_RENDERTARGET_HPP
#define GL_PLAY_RENDERTARGET_HPP
#include <memory>
#include "Definitions.hpp"
#include "Material.hpp"


class RenderTarget {

public:
  RenderTarget(int width, int height);
  ~RenderTarget();

  void activate();
  void render();

static void screenshot_ppm(const char *filename, unsigned int width,
        unsigned int height, GLubyte **pixels) {
    size_t i, j, k, cur;
    const size_t format_nchannels = 3;
    FILE *f = fopen(filename, "w");
    fprintf(f, "P3\n%d %d\n%d\n", width, height, 255);
    *pixels = (GLubyte*) realloc(*pixels, format_nchannels * sizeof(GLubyte) * width * height);
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, *pixels);
    for (i = 0; i < height; i++) {
        for (j = 0; j < width; j++) {
            cur = format_nchannels * ((height - i - 1) * width + j);
            fprintf(f, "%3d %3d %3d ", (*pixels)[cur], (*pixels)[cur + 1], (*pixels)[cur + 2]);
        }
        fprintf(f, "\n");
    }
    fclose(f);
}

private:
  int m_width, m_height;
  GLuint frameBuffer;
  GLuint renderTexture;
  GLuint quad_VertexArrayID;
  GLuint quad_vertexbuffer;

  GLuint depthrenderbuffer;

  std::unique_ptr<Material> mat;
  GLint texID;
};


#endif //GL_PLAY_RENDERTARGET_HPP
