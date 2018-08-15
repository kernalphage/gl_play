#version 330 core

layout (points) in;
layout (points, max_vertices = 16) out;
out vec4 ourColor;

uniform vec4 u_adj[10];

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

void main(){
	vec2 p = gl_in[0].gl_Position.xy;
	vec2 pp = gl_in[0].gl_Position.xy;
	for(int i=0; i < 16; i++){
	    int fun = int(p.x * i * 4623);
	    fun = fun % 4;
	    if(fun == 0){
	        p = handkerchief(p, u_adj[0]);
	    }
	    if(fun == 1){
	        p = sinus(p, u_adj[1]);
	    }
	    if(fun == 2){
	        p = swirl(p, u_adj[2]);
	    }
	    if(fun == 3){
	        p = vec2(-p.x, p.y);
	    }
		gl_Position = vec4(p, 0,1);
		ourColor = vec4(pp, 0,1);
		EmitVertex();
		EndPrimitive();
	}
}