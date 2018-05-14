#ifndef DEFINITIONS_HPP
#define DEFINITIONS_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>
#include <glm/ext.hpp>
#include <string>
#include <ctime>

using namespace glm;

#define SPLAT2(t) t.x, t.y
#define SPLAT3(t) t.x, t.y, t.z
#define SPLAT4(t) t.x, t.y, t.z, t.w

#define SPLAT_OP_2(t, op) t.x op t.y
#define SPLAT_OP_3(t, op) t.x op t.y op t.z
#define SPLAT_OP_4(t, op) t.x op t.y op t.z op t.w


struct rect{
  vec2 tl;
  vec2 br;
};

struct UI_Vertex {
  vec3 pos;
  vec4 color = {1.0f, 0.0f, 1.0f, 1.0f};
};
struct TEX_Vertex{
  vec3 pos;
  vec4 color;
  vec2 uv;
};

struct Util{

static	std::string timestamp(int seed){
	  char mbstr[150];
	   std::time_t t = std::time(nullptr);
	  std::strftime(mbstr, sizeof(mbstr), "%m_%d_%s_%%d.txt", std::localtime(&t));
	  sprintf(mbstr, mbstr, seed);
	  return std::string(mbstr);
	}

};

#endif // DEFINITIONS_HPP