#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aUV;
layout (location = 1) in float aTexSize;
layout (location = 1) in float aWorldSize;

out VS_OUT {
    vec2 uv;
    float texSize;
    float worldSize;
} vs_out;

void main()
{
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0); 
    vs_out.uv = aUV;
    vs_out.texSize = aTexSize;
    vs_out.worldSize = aWorldSize;
}  