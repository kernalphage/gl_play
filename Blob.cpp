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
  return !(pt.x < bounds.tl.x || pt.x > bounds.br.x || pt.y < bounds.tl.y || pt.y > bounds.br.y);
}

float min_dist(const vec2 &pt, vector<Blob *> &v, rect bounds) {
  float curmin = 1000;
  for(auto b : v){
    if (point_in_circle(pt, b->pos, b->r)) return -1.0f;
    float mindist = std::min(distance(pt, b->pos) - b->r, curmin);
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
Partition::gen_poisson(vec2 tl, vec2 br, float rmin, float rmax, int maxSamples, vector<Blob *> &out, float overlap) {
  vector<Blob *> open;

  rect bounds{tl, br};
  vec2 seed = generate_random_point(bounds);

  Blob *s = new Blob{seed, rmax};
  out.push_back(s);
  open.push_back(s);
  add(s);

  //while there's space
  while (!open.empty() && maxSamples > 0) {
    Blob *cur = open.back();
    auto endr = open.end();

    vector<vec2> samples;
    auto scale = 0;
    rect torus{vec2{rmin + cur->r, 0}, vec2{rmax + cur->r, 6.28}};

    //find a couple spots
    int generated = 0;
    for (int i = 0; i < 50; i++) {
      vec2 samp = generate_random_point(torus);
      samp = cur->pos + (vec2{sin(samp.y), cos(samp.y)}) * samp.x;

      vector<Blob *> cur_neighbors;
      neighbors(samp, std::back_inserter(cur_neighbors));
      float r = min_dist(samp, cur_neighbors, bounds);

      if (r > rmin + scale) {
        generated++;
        maxSamples--;
        Blob *n = new Blob{samp, std::min(r + overlap, rmax + scale)};

        open.push_back(n);
        out.push_back(n);
        add(n);
        //n->parent = cur;
        //n->gparent = cur->parent;
        //if(n->gparent != nullptr){
        //    n->ggparent = n->gparent->parent;
        //}
        //cur->child = n;
      }
      if (maxSamples <= 0)
        break;
    }
    // re-add it
    if (generated == 0) {
      auto idx = std::find(open.begin(), open.end(), cur);
      open.erase(idx);
    }
  }
}

void Blob::render(Processing *ctx) {
  // TODO: Make this a ctx->circle(pos, radius, steps)
  const float pi = 3.1415f;
  vec3 threepos{pos, 1};
  UI_Vertex center{threepos, {1, 1, 1, 1}};
  vec4 outer{1.0f, 1.0, 0.0, 0.0};
  float dTheta = (2 * pi) / 16;
  for (float theta = dTheta; theta <= (2 * pi); theta += dTheta) {
    ctx->tri(UI_Vertex{vec3{sin(theta), cos(theta), 1} * r + threepos, outer},
             UI_Vertex{vec3{sin(theta + dTheta), cos(theta + dTheta), 1} * r + threepos, outer}, center);
  }
}