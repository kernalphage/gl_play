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

  void reseed(float domain, std::function<T(vec2)> func);

  ~RandomCache(){
    delete [] m_cache;
  }
  int idx (float row, float col) {
    return ((int)row % m_resolution) * m_resolution + ((int) col % m_resolution);
  }
  T sample(vec2 pos);
//TODO: requires me to understand constexpr if, apply m_cache to a texturebuffer  
 // void apply_texture(int tex_location);
private:
  T* m_cache;
  int m_resolution;
  float m_domain;
};



#endif //GL_PLAY_RANDOMCACHE_HPP
