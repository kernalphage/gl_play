#version 330 core

in vec2 UV;

layout(location = 0) out vec3 color;

uniform sampler2D renderedTexture;
uniform float time;

void main(){
    color = vec3(UV, 1);//texture( renderedTexture, UV ).xyz;
}