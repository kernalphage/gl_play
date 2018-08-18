//
// Created by matt on 8/15/18.
//

#include "Streamline.hpp"
#include "Processing.hpp"

void Streamline::render(Processing *ctx) {
  for(const auto line: lines){
    vector<vec3> spline;
    auto cur = line;
    while(cur != nullptr){
      spline.push_back({SPLAT2(cur->pos), 0});
      cur = cur->next;
    }
    ctx->spline(spline,{1,1,1,1}, {1,0,0,1}, false);
  }
}
