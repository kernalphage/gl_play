//
// Created by matt on 3/5/18.
//

#include "Triangulate.hpp"
using namespace std;

std::vector<Tri> Tri::triangulate(const std::vector<Tri>& seed, float maxDepth, Processing* ctx) {
  vector<Tri> ret{seed};
  size_t cur = 0;

  while( cur < ret.size()){
    Tri curTri = ret[cur];
    int side = longestSide(curTri);

    // 0 __d__ 1
    //   \ | /
    //    \|/
    //     2
    vec3 d =  mix(curTri[side], curTri[side+1], .5f );

    ctx->line(d, curTri[side+2]);

    float newDepth = curTri.depth + 1;
    if(newDepth < maxDepth ) {
      ret.push_back(Tri{curTri[side], curTri[side + 2],     d, newDepth});
      ret.push_back(Tri{curTri[side + 2], d, curTri[side + 1], newDepth});
    }

    cur++;
  }
  return ret;
}

// Longest side on the triangle
int Tri::longestSide(Tri tri) {
  return 0;// random() % 3;
}

bool testTriangle() {

  Tri tri{};
  tri.c = vec3{2,3,0};
  tri[2].y = 4;
  assert(tri.c.y == 4);

  return false;
}
