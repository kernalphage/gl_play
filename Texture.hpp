#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include "Definitions.hpp"
#include <stb/stb_image.h>

class Texture {
public:
    ~Texture(){
        stbi_image_free(pixels);
    }
    void use(GLuint location){
        glActiveTexture(location);
        glBindTexture(GL_TEXTURE_2D, m_id);
    }
    void load(const char* filename){
         pixels = stbi_load(filename, &texWidth, &texHeight, &texChannels, 0);
        // TODO: only gen if needed 
        glGenTextures(1, &m_id);
        glBindTexture(GL_TEXTURE_2D, m_id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texWidth, texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    unsigned char sample(float u, float v, int channel){
        int x = texWidth * u;
        int y = texHeight * v;
        x = glm::clamp(x, 0, texWidth-1);
        y = glm::clamp(y, 0, texHeight-1);
        return pixels[(y * texWidth + x) * texChannels + channel];
    }

private:
    GLuint m_id;
    stbi_uc* pixels;

  int texWidth, texHeight, texChannels;

};


#endif