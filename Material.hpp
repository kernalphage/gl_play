#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include <glad/glad.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

class Material {
public:
  unsigned int ID;
  bool _transparent;
  // constructor generates the shader on the fly
  // ------------------------------------------------------------------------
  Material(const char *vertexPath, const char *fragmentPath, bool transparent, const char* geomPath = nullptr) {
    _transparent = transparent;

    auto loadCode = [](const char* path){
      std::string code;
      std::ifstream sFile;

      sFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
      // Try
      sFile.open(path);
      std::stringstream vShaderStream;
      vShaderStream << sFile.rdbuf();
      sFile.close();
      code = vShaderStream.str();
      // catch
      return code;
    };

    const char *vShaderCode = loadCode(vertexPath).c_str();
    const char *fShaderCode = loadCode(fragmentPath).c_str();
    // 2. compile shaders
    unsigned int vertex, fragment;
    // vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX", vertexPath);
    // fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT",fragmentPath);

    // shader Program
    ID = glCreateProgram();

    if(geomPath != nullptr){
      const char* gShaderCode = loadCode(geomPath).c_str();
      unsigned int geom;
      geom = glCreateShader(GL_GEOMETRY_SHADER);
      glShaderSource(geom, 1, &gShaderCode, NULL);
      glAttachShader(ID, geom);
    }

    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM", "Final Program");
    // delete the shaders as they're linked into our program now and no longer
    // necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);
  }
  // activate the shader
  // ------------------------------------------------------------------------
  void use() {
    glUseProgram(ID);
    if (_transparent) {
      glEnable(GL_BLEND);

    //  glBlendFunc(GL_ONE, GL_ONE);
      //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      //glBlendFunc(GL_SRC_ALPHA, GL_ONE);
      glBlendEquation(GL_FUNC_ADD);
    }
  }
  // utility uniform functions
  // ------------------------------------------------------------------------
  void setBool(const std::string &name, bool value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
  }
  // ------------------------------------------------------------------------
  void setInt(const std::string &name, int value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
  }
  // ------------------------------------------------------------------------
  void setFloat(const std::string &name, float value) const {
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
  }

private:
  // utility function for checking shader compilation/linking errors.
  // ------------------------------------------------------------------------
  void checkCompileErrors(unsigned int shader, std::string type, const char* path) {
    int success;
    char infoLog[1024];
    if (type != "PROGRAM") {
      glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
      if (!success) {
        glGetShaderInfoLog(shader, 1024, NULL, infoLog);
        std::cout
            << path
            << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n"
            << infoLog
            << "\n -- --------------------------------------------------- -- "
            << std::endl;
      }
    } else {
      glGetProgramiv(shader, GL_LINK_STATUS, &success);
      if (!success) {
        glGetProgramInfoLog(shader, 1024, NULL, infoLog);
        std::cout
            << path
            << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n"
            << infoLog
            << "\n -- --------------------------------------------------- -- "
            << std::endl;
      }
    }
  }
};
#endif