//
// Created by matt on 11/14/18.
//

#ifndef GL_PLAY_RANDOMCACHE_HPP
#define GL_PLAY_RANDOMCACHE_HPP
#include "Definitions.hpp"
#include "Random.hpp"
template<typename T>
class RandomCache {
public:
  RandomCache(int resolution, float domain, std::function<T(vec2)> func){
    m_cache = new T[resolution * resolution];
    m_resolution = resolution;
    reseed(domain, func);
  }
  void reseed(float domain, std::function<T(vec2)> func){
    m_domain = domain;
    for(int i=0; i < m_resolution * m_resolution; i++){
      int row = i % m_resolution;
      int col = i / m_resolution;
      vec2 pos = vec2(Util::rangeMap(col, 0, m_resolution, 0, domain), Util::rangeMap(row, 0, m_resolution, 0, domain)  );
      m_cache[idx(row, col)] = func(pos);
    }
  }

  ~RandomCache(){
    delete [] m_cache;
  }
  int idx (float row, float col) {
    return ((int)row % m_resolution) * m_resolution + ((int) col % m_resolution);
  }
  T sample(vec2 pos){
    float row = Util::rangeMap(pos.y, 0, m_domain, 0, m_resolution, true);
    float col = Util::rangeMap(pos.x, 0, m_domain, 0, m_resolution, true); // todo: bilinear interpolation
    float drow = glm::fract(row);
    float dcol = glm::fract(col);
  if(glm::isnan(row) || glm::isnan(col)){
    row = 0;
    col = 0;
  }
    T a = m_cache[idx(floor(row), floor(col))];
    T b = m_cache[idx(floor(row), ceil(col))];
    T c = m_cache[idx(ceil(row), floor(col))];
    T d = m_cache[idx(ceil(row), ceil(col))];
    T row1 = glm::lerp(a,b,dcol);
    T row2 = glm::lerp(c,d,dcol);

    return glm::lerp(row1, row2, drow);
  }
private:
  T* m_cache;
  int m_resolution;
  float m_domain;
};


#endif //GL_PLAY_RANDOMCACHE_HPP
