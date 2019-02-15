#version 330 core

in vec2 UV;

layout(location = 0) out vec4 color;

uniform sampler2D renderedTexture;
uniform sampler2D tonemap;
uniform float gamma;
uniform float energy;
uniform float texAmt;

void main(){
    vec4 val = texture( renderedTexture, UV );
    vec4 lum =  log(val) * gamma;
   lum = max(lum, vec4(0));
    lum = pow(lum, vec4(energy) );
    lum = clamp(lum, 0., 1.);

    //lum.y =1 - lum.y;

    color = mix(lum, texture(tonemap,  lum.rg), texAmt);
}
