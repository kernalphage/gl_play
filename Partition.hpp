//
// Created by matt on 10/7/17.
//

#ifndef WRITER_PARTITION_HPP
#define WRITER_PARTITION_HPP
#include <vector>
using namespace std;

template<class T>
class Partition{

  public:
    Partition(vec2 origin, vec2 extents, float maxSize):
        _offset(origin), _scale(1 / (maxSize)), _numgrids((int) ceil(extents.x / maxSize)){
        for(int i=0; i < _numgrids; i++){
            for(int j=0; j < _numgrids; j++){
                _grid.push_back(vector<T>{});
                _heatmap.push_back(0);
            }
        }
    }

    typedef std::function<vec2(vec2)> DistanceFN;

    void gen_random(vec2 tl, vec2 br, DistanceFN sampleSize, int maxSamples, std::vector<T>& out);
    void gen_poisson(vec2 tl, vec2 br, DistanceFN sampleSize, int maxSamples, vector<T>& out, float overlap);
    int add(T t){
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
    int index(T b){
        return index_impl(std::is_pointer<T>(), b);
    }

    int index_impl(std::true_type, T b){
        return index(b->pos);
    }
    int index_impl(std::false_type, T b){
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
private:
    const vec2 _offset;
    float _scale;
    int _numgrids;
    vector<vector<T>> _grid;
    vector<T> _all;

    vector<int> _heatmap;
  };

#endif //WRITER_PARTITION_HPP
