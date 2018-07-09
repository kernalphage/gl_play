//
// Created by matt on 3/29/18.
//

#ifndef GL_PLAY_BLOB_HPP
#define GL_PLAY_BLOB_HPP
#include "Definitions.hpp"
#include <vector>
#include <iostream>

using namespace std;

class Processing;

struct Blob{
  Blob(vec2 _pos, float _r): pos(_pos), r(_r){ }
  void render(Processing *ctx, vec4 color, float thickness);


  vec2 pos;
  float r;
  //int depth;
};

struct Partition{

public:
  Partition(vec2 origin, vec2 extents, float maxSize):
      _offset(origin), _scale(1 / (maxSize)), _numgrids((int) ceil(extents.x / maxSize)){
    for(int i=0; i < _numgrids; i++){
      for(int j=0; j < _numgrids; j++){
        _grid.push_back(vector<Blob>{});
        _heatmap.push_back(0);
      }
    }
  }

  typedef std::function<vec2(vec2)> DistanceFN;

  void gen_random(vec2 tl, vec2 br, DistanceFN sampleSize, int maxSamples, std::vector<Blob>& out);
  void gen_poisson(vec2 tl, vec2 br, DistanceFN sampleSize, int maxSamples, vector<Blob>& out, float overlap);
  int add(Blob t){
    int i = index(t);
    _all.push_back(t);
    _grid[i].push_back(t);
    return i;
  }
  template<typename OutputIterator>
  void neighbors(vec2  v, OutputIterator n_out){
    int i = index(v);
    int neigh[] = {-(_numgrids+1), -_numgrids, -_numgrids + 1,
                   -1			 ,      0 	 , 		1,
                   _numgrids-1, 		_numgrids, _numgrids+1};
    for(auto n : neigh){
      n = n+i;
      if(n <0 || n  >= _numgrids * _numgrids) continue;
      for(auto b : _grid[n]){
        n_out = b;
      }
    }
  }
  int index(Blob b){
    return index(b.pos);
  }

  int index(const vec2 v){
    vec2 v2 = v - _offset;
    int ex = (int) floor((v2.x) * _scale);
    int wy = (int) floor((v2.y) * _scale);
    ex = glm::min(glm::max(ex, 0), _numgrids-1);
    wy = glm::min(glm::max(wy, 0), _numgrids-1);
    int idx = ex + wy * _numgrids;
    _heatmap[idx]++;
    return idx;
  }
  void dump(){
    for(int i=0; i < _numgrids; i++) {
      for (int j = 0; j < _numgrids; j++) {
        cout<< _heatmap[i + j * _numgrids]<<"\t";
      }
      cout<<endl;
    }
  }

  const vec2 _offset;
  float _scale;
  int _numgrids;
  vector<vector<Blob>> _grid;
  vector<Blob> _all;

  vector<int> _heatmap;
};

#endif //GL_PLAY_BLOB_HPP
