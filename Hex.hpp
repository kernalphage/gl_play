//
// Created by matt on 4/29/18.
//

#ifndef GL_PLAY_HEX_HPP
#define GL_PLAY_HEX_HPP

#include <glm/glm.hpp>
#include <iostream>
#include <algorithm>
#include <vector>
#include <numeric>
#include <string>
#include <sstream>
#include <range/v3/all.hpp>

#include "glm/ext.hpp"
using namespace std;
using namespace glm;

class Hex : public ivec3{
public:
  Hex(): ivec3() {}
  Hex(ivec3 v): ivec3(v){}
  Hex(int i, int j) : ivec3(i,j,-i-j){}
  Hex(float i, float j, float k){
    int rx = (int) round(i);
    int ry = (int) round(j);
    int rz = (int) round(k);

    float x_diff = abs(rx - i);
    float y_diff = abs(ry - j);
    float z_diff = abs(rz - k);

    if(x_diff > y_diff && x_diff > z_diff){
      rx = -ry-rz;
    }
    else if( y_diff > z_diff){
      ry = -rx-rz;
    }
    else{
      rz = -rx-ry;
    }

    x = rx;
    y = ry;
    z = rz;
  }

  int distance(Hex b) const {
    return (abs(x - b.x) + abs(y - b.y) + abs(z - b.z)) / 2;
  }
  static vector<Hex> line(const Hex a,const Hex b){
    int N = a.distance(b);
    cout<<"numpts " << N+1;
    return {};//
        // ranges::view::linear_distribute(0.0f, 1.0f, N+1) | ranges::view::transform(
        //[&](auto t){ return hex_lerp(a, b, t);}
    //);
  }
  static float lerp(const float a, const float b, const float t){
    return a + (b - a) * t;
  }
  static Hex hex_lerp(const Hex a, const Hex b,const float t){
    return Hex{ lerp(a.x, b.x, t)+.000001f,
                lerp(a.y, b.y, t)+.000001f,
                lerp(a.z, b.z, t)-.000002f };
  }
};
static const vector<Hex> axial_directions{
    Hex(+1,  0), Hex(+1, -1), Hex( 0, -1),
    Hex(-1,  0), Hex(-1, +1), Hex( 0, +1)
};

std::vector<Hex> neighbors(Hex c){
  return axial_directions; // | ranges::view::transform( [=](auto n){return c+n;});
};


string print_map(int n, std::vector<Hex> v, vector<Hex> pts){
  int h = n*2 - 1;
  stringstream sb;
  for(int i=0; i < h; i++){
    int slope = abs((i+1) - n);
    int numcells = n+(n-1)- abs((i+1) - n);


    int ex = - std::min(i, n);
    int wy = 1-(n-i) ;
    for(int s =0; s < slope; s++){
      sb<<" ";
    }
    for(int j=0; j < numcells; j++){
      Hex pos{ex+j, wy};

      if(ranges::find(pts, pos) != pts.end()){
        sb<<"# ";
      }else if(ranges::find(v, pos) != v.end()){
        sb<<"* ";
      }
      else{
        sb<<"_ ";
      }
    }
    for(int s =0; s < slope; s++){
      sb<<" ";
    }
    sb<<wy<<endl;
  }
  return sb.str();
}

class TileObject{
  Hex position;
  int storage = 0;
};

#endif //GL_PLAY_HEX_HPP
