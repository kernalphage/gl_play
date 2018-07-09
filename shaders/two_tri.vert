#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec2 vertex_pos;
layout(location = 1) in vec2 vertex_uv;

// Output data ; will be interpolated for each fragment.
out vec2 UV;

void main(){
	gl_Position =  vec4(vertex_pos, 0,1);
	UV = vertex_uv;
}
