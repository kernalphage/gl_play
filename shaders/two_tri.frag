#version 330 core

in vec2 UV;

layout(location = 0) out vec4 color;

uniform sampler2D renderedTexture;
uniform sampler2D tonemap;
uniform float gamma;
uniform float energy;

void main(){
    vec4 val = texture( renderedTexture, UV );
    vec4 lum =  log(val) * gamma;
    lum = clamp(lum, 0., 1.);
    lum = pow(lum, vec4(energy) );

    //lum.y =1 - lum.y;
    color = vec4(lum.rgb, 1);// texture(tonemap, lum);
}
