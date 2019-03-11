//
// Created by matt on 11/10/18.
//
#include "Material.hpp"
#include <memory>
#include <string>
#include <map>

#include "fmt/format.h"

using namespace std;
map<string, shared_ptr<Material>> Material::s_materials;
Material::Material(std::string vertexPath, std::string fragmentPath, bool transparent, string geomPath) {
  _transparent = transparent;

  auto loadCode = [](std::string path){
    std::string code;
    std::ifstream sFile;

    sFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    // Try
    sFile.open(path);
    std::stringstream vShaderStream;
    vShaderStream << sFile.rdbuf();
    sFile.close();
    // catch
    return vShaderStream.str();;
  };
  unsigned int vertex=0, fragment=0, geom=0;
  ID = glCreateProgram();
  if(! vertexPath.empty()) {
    std::string vShader = loadCode(vertexPath);
    const char *vShaderCode = vShader.c_str();
    // vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX", vertexPath);
    glAttachShader(ID, vertex);
  }

  if(! fragmentPath.empty()) {
    std::string fShader = loadCode(fragmentPath);
    const char *fShaderCode = fShader.c_str();
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT", fragmentPath);
    glAttachShader(ID, fragment);
  }

  if(! geomPath.empty()){
    std::string geomCode = loadCode(geomPath);
    const char* gShaderCode = geomCode.c_str();
    geom = glCreateShader(GL_GEOMETRY_SHADER);
    glShaderSource(geom, 1, &gShaderCode, NULL);
    glCompileShader(geom);
    checkCompileErrors(geom, "GEOM",fragmentPath);
    glAttachShader(ID, geom);
  }

  glLinkProgram(ID);
  checkCompileErrors(ID, "PROGRAM", "Final Program");
  // delete the shaders as they're linked into our program now and no longer
  // necessary
  glDeleteShader(vertex);
  glDeleteShader(fragment);
  glDeleteShader(geom);
}

void Material::use() {
  glUseProgram(ID);
  if (_transparent) {
    glEnable(GL_BLEND);

    //  glBlendFunc(GL_ONE, GL_ONE);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glBlendEquation(GL_FUNC_ADD);
  } else {
    glDisable(GL_BLEND);
  }
}

void Material::checkCompileErrors(unsigned int shader, std::string type, std::string path) {
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

void Material::loadMaterials(std::string jsonloc) {

  rapidjson::Document doc;
  std::ifstream sFile(jsonloc);
  std::stringstream vShaderStream;

  if(!sFile.is_open()){
    return;
  }
  vShaderStream << sFile.rdbuf();
  sFile.close();
  // catch
  map<string, shared_ptr<Material>> s_materials;
  std::string jsonContents=vShaderStream.str();
  auto load = vShaderStream.str();
  doc.Parse(load.c_str());
  auto mats = doc["materials"].GetArray();
  for (rapidjson::Value::ConstValueIterator itr = mats.Begin(); itr != mats.End(); ++itr)
  {
    auto material = itr->GetObject();
    std::cout<<material["name"].GetString()<<std::endl;
    string vert, frag, geom;
    bool transparent;
    vert = fmt::format("shaders/{0}.vert", material["vert"].GetString());
    frag = fmt::format("shaders/{0}.frag", material["frag"].GetString());
    geom = material.FindMember("geom") == material.MemberEnd() ? "" :  fmt::format("shaders/{0}.geom", material["geom"].GetString());
    transparent = material["transparent"].GetBool();

    s_materials.emplace(material["name"].GetString(), std::make_shared<Material>(vert, frag, transparent, geom) );
  }

}

std::shared_ptr<Material>
Material::getMaterial(std::string name) {
  return s_materials.find(name)->second;
}
