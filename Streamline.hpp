//
// Created by matt on 8/15/18.
//

#ifndef GL_PLAY_STREAMLINE_HPP
#define GL_PLAY_STREAMLINE_HPP

#include "Definitions.hpp"
#include "Partition.hpp"
#include <vector>
#include <numeric>
#include "Partition.hpp"

using namespace std;
class Processing;

class Streamline {
  struct Node{
    Node* next;
    vec2 pos;
    bool open;
  };

  struct Line{
    Node* root;
  };
public:

  ~Streamline(){
    //hoo boy
  }

  bool isvalid(Node* n){
    vec2 pt = n->pos;

    if(pt.x < 0 || pt.y < 0){
      return false;
    }
    if(pt.x > width || pt.y > height){
      return false;
    }
    vector<Node *> neigh;
    p.neighbors(pt, std::back_inserter(neigh));


    const auto distfn = [=]( Node* next ){ return glm::distance2(next->pos, pt);};
    float nodeDist = std::accumulate(neigh.begin(), neigh.end(), distfn(neigh[0]), [=](float d, Node* n){
      return std::min(d, distfn(n));
    });

    return nodeDist > minDist;

  };

  Node* next(Node* pos, bool backwards){
    return new Node{nullptr, pos->pos + vec2{.05f, .05}, false};
  };

  Node* stream_point(Node* start){

    Node* cur = start;
    bool backwards = false;
    while(cur){
      auto nextp = next(cur, backwards);
      if(!isvalid(nextp)) {
        //leak!
        if(!backwards){
          backwards = true;
          cur = start;
          continue;
        }
        else{
          break;
        }
      }

      p.add(cur);

      if(backwards){
        nextp->next = cur;
      }else{
        cur->next = nextp;
      }
      cur = nextp;
    }

    return cur; //the "start" of our line
  }

  void render(Processing* ctx);

private:

  vector<Node*> lines;
  Partition<Node*> p;
  float width;
  float height;
  float minDist;
};


#endif //GL_PLAY_STREAMLINE_HPP
