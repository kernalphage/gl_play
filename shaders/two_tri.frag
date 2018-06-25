#version 330 core

in vec2 UV;

layout(location = 0) out vec4 color;

uniform sampler2D renderedTexture;
uniform sampler2D tonemap;
uniform float gamma;
uniform float energy;

void main(){
    vec4 val = texture( renderedTexture, UV );
    vec4 lum =  pow(val/energy, vec4(gamma));
    lum = clamp(lum, 0., 1.);
    //lum.y =1 - lum.y;
    color = texture(tonemap, lum);
}
