#version 330 core

in vec2 UV;

layout(location = 0) out vec4 color;

uniform sampler2D renderedTexture;
uniform float gamma;
uniform float energy;

void main(){
    vec4 val = texture( renderedTexture, UV );
    vec4 lum =  pow(val, vec4(gamma))/energy;
    color = lum;
}
