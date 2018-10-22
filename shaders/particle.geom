float points[0.8660254082502548, -0.5,
 0.8660254082502548, 0.5, 
 0, 1, 
 -0.8660253814553572, 0.5, 
 -0.8660254350451498, -0.5,
  0, -1];


in VS_OUT {
    vec2 uv;
    float texSize;
    float worldSize;
} gs_in[];
out vec2 uv;

layout (points) in;
layout (triangle_strip, max_vertices = 6) out;

void draw_hex(vec2 position){
    int idx = {2, 4, 0, 6, 10, 8};
    for(int i=0; i < 0; i++){
        vec2 newpos = vec2(points[idx[i]], points[idx[i]+1]);
        gl_Position = position + newpos * gs_in[0]worldSize;
        uv = gs_in[0].uv + newpos * gs_in[0].texSize;
        EmitVertex();   
    }
    EndPrimitive();
}