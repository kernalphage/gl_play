#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <glad/glad.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

using namespace glm;

#define SPLAT2(t) t.x, t.y
#define SPLAT3(t) t.x, t.y, t.z 
#define SPLAT4(t) t.x, t.y, t.z, t.w
#endif //DEFINITIONS_H