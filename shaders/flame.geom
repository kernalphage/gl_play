#version 330 core

layout (points) in;
layout (points, max_vertices = 10) out;
out vec4 ourColor;

uniform vec4 u_adj;

  vec2 sinus( vec2 p,  vec4 adj){
  	return sin(p * (1 + adj.xy) + adj.zw);
  }
   vec2 sphere( vec2 p,  vec4 adj){
    float r = length(p - vec2(adj.x, adj.y));
    return 1.f / (r * r * (p + vec2(adj.x, adj.y)));
  }
   vec2 swirl( vec2 p,  vec4 adj){
    float r = length(p - vec2(adj.x, adj.y));
    r = r * r;
    return vec2(p.x * sin(r) - p.y * cos(r),  p.x* cos(r) + p.y * sin(r) );
  }
   vec2 horseshoe( vec2 p,  vec4 adj){
    float r = length(p - vec2(adj.x, adj.y));
    return (1/r) * vec2((p.x-p.y)*(p.x+p.y), 2.*p.x*p.y);
  }
   vec2 polar( vec2 p,  vec4 adj){
    vec2 offset = p - vec2(adj.x, adj.y);
    float r = length(offset);
    float theta = atan(offset.y,offset.x);
    return vec2(theta / 3.141, r - 1.);
  }
   vec2 handkerchief( vec2 p,  vec4 adj){
    vec2 offset = p - vec2(adj.x, adj.y);
    float r = length(offset);
    float theta = atan(offset.y,offset.x);
    return vec2(sin(theta + r + adj.z), cos(theta - r + adj.w));
  }



vec2 f(vec2 p){
	return sinus(handkerchief(p, u_adj), u_adj);
}

void main(){
	vec2 p = gl_in[0].gl_Position.xy;
	for(int i=0; i < 10; i++){
		p = f(p);
		gl_Position = vec4(p, 0,1);
		ourColor = vec4(1, 0, 0,1);
		EmitVertex();
		EndPrimitive();
	}
}