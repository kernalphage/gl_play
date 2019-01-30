#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aCol;
uniform mat4 worldTransform;
out vec4 ourColor;

void main()
{
    gl_Position = worldTransform * vec4(aPos.xy, 0, 1.0);
    ourColor = aCol;
}