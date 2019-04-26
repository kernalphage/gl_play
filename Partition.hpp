//
// Created by matt on 10/7/17.
//

#ifndef WRITER_PARTITION_HPP
#define WRITER_PARTITION_HPP
#include <vector>
using namespace std;

class Line;


template<class T>
class Partition{

  public:
    Partition():
        _offset({0,0}){

    }
  // TODO: I don't like duplicating resize and this constructor
    Partition(vec2 origin, vec2 extents, float maxSize):
        _offset(origin), _scale(1 / (maxSize)), _numgrids((int) ceil(extents.x / maxSize)){
        for(int i=0; i < _numgrids; i++){
            for(int j=0; j < _numgrids; j++){
                _grid.push_back(vector<T>{});
            }
        }
    }
    void resize(vec2 origin, vec2 extents, float maxSize){
        _offset = origin;
        _scale = 1/(maxSize);
        _numgrids = (int) ceil(extents.x / maxSize);
        _grid.clear();
        _all.clear();
        for(int i=0; i < _numgrids; i++){
            for(int j=0; j < _numgrids; j++){
                _grid.push_back(vector<T>{});
            }
        }
    }
    void clear(){
        for(vector<T>& row : _grid){
            row.clear();
        }
        _all.clear();
    }

    typedef std::function<vec2(vec2)> DistanceFN;

    void gen_random(vec2 tl, vec2 br, DistanceFN sampleSize, int maxSamples, std::vector<T>& out);
    void gen_poisson(vec2 tl, vec2 br, DistanceFN sampleSize, int maxSamples, vector<T>& out, float overlap);
    vector<int> add(T t){

        _all.push_back(t);
        vector<int> buckets = index(t);
        for(int i : buckets){     
            _grid[i].push_back(t);
        }
        return buckets; // mayybe i shouldnt return here? 
    }
    template<typename OutputIterator>
    void neighbors(vec2  v, OutputIterator n_out){
        vector<int> buckets = index(v);
        for(int i : buckets){            
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
    }
    vector<int> index(T b)
    {
        return index_impl(std::is_pointer<T>(), b);
    }

    vector<int> index_impl(std::true_type, T b)
    {
        return index(b->pos);
    }
    vector<int> index_impl(std::false_type, T b)
    {
        return index(b.pos);
    }
    vector<int> index(const vec2 v){
        vec2 v2 = v - _offset;
        int ex = (int) floor((v2.x) * _scale);
        int wy = (int) floor((v2.y) * _scale);
        ex = glm::min(glm::max(ex, 0), _numgrids-1);
        wy = glm::min(glm::max(wy, 0), _numgrids-1);
        int idx = ex + wy * _numgrids;
        return {idx}; // no, this doesn't make a list of size 23
    }
  vector<T> _all;
private:
  vec2 _offset;
  float _scale;
  int _numgrids;
  vector<vector<T>> _grid;

  };

#endif //WRITER_PARTITION_HPP
