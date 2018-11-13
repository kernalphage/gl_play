#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include "Definitions.hpp"
#include <stb/stb_image.h>

class Texture {
public:
    void use(GLuint location){
        glActiveTexture(location);
        glBindTexture(GL_TEXTURE_2D, m_id);
    }
    void load(const char* filename){
        int texWidth, texHeight, texChannels;

        stbi_uc* pixels = stbi_load(filename, &texWidth, &texHeight, &texChannels, 0);
        // TODO: only gen if needed 
        glGenTextures(1, &m_id);
        glBindTexture(GL_TEXTURE_2D, m_id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texWidth, texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
        stbi_image_free(pixels);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
private: 
    GLuint m_type;
    GLuint m_id;
};


#endif