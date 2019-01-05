#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include <memory>
#include <string>
#include <glad/glad.h>
#include "Definitions.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <map>
class Material {
public:
  unsigned int ID;
  bool _transparent;

  static std::shared_ptr<Material> getMaterial(std::string name);
  static void loadMaterials(std::string jsonloc);

  Material(std::string vertexPath, std::string fragmentPath, bool transparent, std::string  geomPath = "");

  void use();
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
  void setVec(const std::string &name, glm::vec4 value) const {
    glUniform4fv(glGetUniformLocation(ID, name.c_str()),1, (float*) &value);
  }
  void setMat4x4(const std::string &name, glm::mat4 mat) const {
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), GL_TRUE, 1, glm::value_ptr(mat));
  }
private:

  static std::map<std::string, std::shared_ptr<Material>> s_materials;
  // utility function for checking shader compilation/linking errors.
  // ------------------------------------------------------------------------
  void checkCompileErrors(unsigned int shader, std::string type, std::string  path);
};
#endif