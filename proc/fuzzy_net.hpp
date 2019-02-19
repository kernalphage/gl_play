//
// Created by matt on 12/19/18.
//

#ifndef GL_PLAY_FUZZY_NET_HPP
#define GL_PLAY_FUZZY_NET_HPP
#include <Definitions.hpp>
#include <Random.hpp>
#include <vector>
using namespace std;

class FuzzNet{
  void draw(){
    int numRings;
    float ringDist;
    int ptsPerRing;
    float ringSkew;
    float minRadius;
    vector<vector<vec4>> objects;
    float di = 6.28 / numRings;
    float maxRadius = minRadius + numRings + ringDist;

    for(int i= 0; i < numRings; i++){
      vector<vec4> ptsa;
      vector<vec4> ptsb;
      for(int j=0; j < ptsPerRing; j++){
        // ring
        float theta = Random::range(6.28);
        // ringSkew + (i * ringDefocus)?
        float radius = Random::nextGaussian() * ringSkew + i * ringDist;
        ptsa.push_back(vec4{sin(theta), cos(theta), 1, 0} * radius);

        // ray
        theta = (i * di)+ Random::nextGaussian() * ringSkew;
        radius = Random::range(minRadius, maxRadius);
        ptsb.push_back(vec4{sin(theta), cos(theta), 0, 1} * radius);
      }
      objects.push_back(ptsa);
      objects.push_back(ptsb);
    }



    for(auto&o : objects){
      for(auto&p : o){
      p.xy =
    }
  }
};


#endif //GL_PLAY_FUZZY_NET_HPP
