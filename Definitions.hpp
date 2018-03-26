#ifndef DEFINITIONS_HPP
#define DEFINITIONS_HPP

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

using namespace glm;

#define SPLAT2(t) t.x, t.y
#define SPLAT3(t) t.x, t.y, t.z
#define SPLAT4(t) t.x, t.y, t.z, t.w

struct UI_Vertex {
  vec3 pos;
  vec4 color = {1.0f, 0.0f, 1.0f, 1.0f};
};

#endif // DEFINITIONS_HPP