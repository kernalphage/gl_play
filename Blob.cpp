//
// Created by matt on 3/29/18.
//

#include "Blob.hpp"
#include "Random.hpp"
#include "Processing.hpp"
#include <glm/ext.hpp>

// rect
// generate_random_point
// min_dist
// make_circle?

vec2 generate_random_point(rect b) {
  return {Random::range(b.tl.x, b.br.x),
          Random::range(b.tl.y, b.br.y)};
}

bool point_in_circle(vec2 pt, vec2 cpt, float cr) {
  return length(pt - cpt) < cr;
}

bool point_in_rectangle(vec2 pt, rect bounds) {
  return (pt.x > bounds.tl.x && pt.x < bounds.br.x && pt.y > bounds.tl.y && pt.y < bounds.br.y);
}

float min_dist(const vec2 &pt, vector<Blob> &v, rect bounds) {
  float curmin = 1000;
  if (!point_in_rectangle(pt, bounds)) {
    curmin = -1;
  }
  for(auto b : v){
    if (point_in_circle(pt, b.pos, b.r)) return -1.0f;
    float mindist = std::min(distance(pt, b.pos) - b.r, curmin);
    curmin = mindist;
  };

  return curmin;
  float initial = 0;
  if (!point_in_rectangle(pt, bounds)) {
    initial = -1;
  } else {
    float topleft = std::min(pt.x - bounds.tl.x, pt.y - bounds.tl.y);
    float botright = std::min(bounds.br.x - pt.x, bounds.br.y - pt.y);
    initial = std::min(topleft, botright);
  }

}

void
Partition::gen_poisson(vec2 tl, vec2 br, DistanceFN distFN, int maxSamples, vector<Blob> &out, float overlap) {
  vector<Blob> open;

  rect bounds{tl, br};
  vec2 seed = generate_random_point(bounds);

  Blob s = Blob{seed, distFN(seed).y};
  out.push_back(s);
  open.push_back(s);
  add(s);

  //while there's space
  while (!open.empty() && maxSamples > 0) {
    Blob cur = open.back(); open.pop_back();
    auto endr = open.end();

    vector<vec2> samples;
    vec2 radii = distFN(cur.pos);
    float rmin = radii.x;
    float rmax = radii.y;

    rect torus{vec2{rmin + cur.r, 0}, vec2{rmax + cur.r, 6.28}};

    //find a couple spots
    int generated = 0;
    for (int i = 0; i < 30; i++) {
      vec2 samp = generate_random_point(torus);
      samp = cur.pos + (vec2{sin(samp.y), cos(samp.y)}) * samp.x;

      vector<Blob> cur_neighbors;
      neighbors(samp, std::back_inserter(cur_neighbors));
      float r = min_dist(samp, cur_neighbors, bounds);

      if (r > rmin) {
        generated++;
        maxSamples--;
        Blob n = Blob{samp, std::min(r + overlap, rmax)};

        open.push_back(n);
        out.push_back(n);
        add(n);
      }
      if (maxSamples <= 0)
        break;
    }
    // re-add it
    if (generated != 0) {
      open.push_back(cur);
    }
  }
}

void Blob::render(Processing *ctx, vec4 inner, vec4 outer) {
  // TODO: Make this a ctx->circle(pos, radius, steps)
  const float pi = 3.1415f;
  vec3 threepos{pos, 1};
  float dTheta = (2 * pi) / 16;
  auto cPos = [=](float t, float radius){ return vec3{sin(t), cos(t), 1} * radius + threepos;};
  for (float theta = dTheta; theta <= (2 * pi); theta += dTheta) {
    ctx->quad(UI_Vertex{cPos(theta, r), outer},
    		UI_Vertex{cPos(theta + dTheta, r), outer},
    		UI_Vertex{cPos(theta + dTheta, r  - .005f), outer},
    		UI_Vertex{cPos(theta, r - .005f), outer});
  }
}