//
// Created by matt on 3/29/18.
//

#include "Blob.hpp"
#include "Random.hpp"
#include "Processing.hpp"
#include <glm/ext.hpp>
#include <imgui_impl/imgui.h>
#include <fmt/format.h>

// rect
// generate_random_point
// min_dist
// make_circle?



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


// renders thick o's so you can control the line width
void Blob::render(Processing *ctx, vec4 color, float thickness) {

	thickness = std::min(thickness, r);
  const float pi = 3.1415f;
  vec3 threepos{pos, 1};
  float dTheta = (2 * pi) / 16;
  auto cPos = [=](float t, float radius){ return vec3{sin(t), cos(t), 1} * radius + threepos;};
  for (float theta = dTheta; theta <= (2 * pi); theta += dTheta) {
    ctx->quad(UI_Vertex{cPos(theta, r), color},
    		UI_Vertex{cPos(theta + dTheta, r), color},
    		UI_Vertex{cPos(theta + dTheta, r  - thickness), color},
    		UI_Vertex{cPos(theta, r - thickness), color});
  }
}




void BlobPlacer::gen_poisson(vec2 tl, vec2 br, BlobPlacer::DistanceFN distFN, int maxSamples, vector<Blob> &out,
                             float overlap){
  Partition<Blob> p;
  p.resize({-1,-1}, {2,2}, _maxRadius);
  vector<Blob> open;
  rect bounds{tl, br};
  vec2 seed;
  Blob s(seed, distFN(seed).y);
  int bailout = 1000;
  while(s.r <= 0 && bailout-->0){
    seed = Random::random_point(tl, br);
    s = Blob(seed, distFN(seed).y);
  }
  out.push_back(s);
  open.push_back(s);
  p.add(s);

  //while there's space
  while (!open.empty() && maxSamples > 0) {
    Blob cur = open.back(); open.pop_back();

    vector<vec2> samples;
    vec2  radii = distFN(cur.pos);
    float rmin = radii.x;
    float rmax = radii.y;

    // TODO: I fucked up something here and the circles don't look random anymore
    //find a couple spots
    int generated = 0;
    for (int i = 0; i < 60; i++) {
      vec2 samp =  Random::random_point(vec2{rmin + cur.r, 0}, vec2{rmax + cur.r, 6.28});
      //	 samp.x = rmax + cur.r;
      samp = cur.pos + (vec2{sin(samp.y), cos(samp.y)}) * samp.x;

      vector<Blob> cur_neighbors;
      p.neighbors(samp, std::back_inserter(cur_neighbors));
      float r = min_dist(samp, cur_neighbors, bounds);

      radii = distFN(samp);

      float tmpMin = radii.x;

      if (r > tmpMin && tmpMin != 0) {
        generated++;
        maxSamples--;
        Blob n = Blob{samp, std::min(r + overlap, rmax)};

        open.push_back(n);
        out.push_back(n);
        p.add(n);
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

void BlobPlacer::render(Processing *ctx) {
  vector<Blob> blobs;
  auto distFN = [=](vec2 pos){
    auto idx = _seed %14;
    auto ppos = pos ;
    ppos.y *=-1;
    ppos = (ppos * .5f + vec2(.5f, .5f) );
    auto dist = Util::median(distField[idx].sample(ppos.x, ppos.y, 0) , distField[idx].sample(ppos.x, ppos.y, 1) ,distField[idx].sample(ppos.x, ppos.y, 2) );
    if(dist < _maxTextDist) {
      return vec2{0,0};
    }
    dist = Util::rangeMap(dist, _maxTextDist, 255, _maxRadius, _minRadius, true);
    return vec2(glm::max(_minRadius, dist * _radiusSkew), dist);
  };
  Random::seed(_seed);
  gen_poisson(vec2{-1,-1}, vec2{1,1}, distFN, _maxSamples, blobs, _overlap);

  for(auto b : blobs){
    ctx->ngon(b.pos, b.r, 12, {10,10,0,10}, {10,0,0,10});
  }
}



void BlobPlacer::imSettings(bool &redraw, bool &clear) {

  static bool doonce = false;
  if(doonce == false){
    doonce = true;
    redraw = true;
    for(int i=0; i < 14; i++){
      auto filename = fmt::format("sdf_text/{0}_fixed.png", numbers[i]);
      distField[i].uploadToGPU = false;
      distField[i].load(filename.c_str());
    }
  }
  redraw |= ImGui::InputInt("seed", &_seed);
  if(ImGui::Button("prevJump")) {
    redraw = true;
    _seed -= 14;
  }
  ImGui::SameLine();
  if(ImGui::Button("nextJump")) {
    redraw = true;
    _seed += 14;
  }

  redraw |= ImGui::SliderInt("minTextDist", &_maxTextDist, 0, 255);
  redraw |= ImGui::SliderFloat("Max Radius", &_maxRadius, _minRadius, .1);
  redraw |= ImGui::SliderFloat("min Radius", &_minRadius, 0, _maxRadius);
  redraw |= ImGui::SliderFloat("radiusSkew", &_radiusSkew, 0, 1);
  redraw |= ImGui::InputFloat("inset", &_overlap, -.1f, .1f);
  redraw |= ImGui::InputInt("MaxSamples", &_maxSamples, 1, 10240);


  clear = redraw;
}
