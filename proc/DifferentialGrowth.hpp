#ifndef DifferentialGrowth_H
#define DifferentialGrowth_H

#include <vector>
#include "Definitions.hpp"
#include "Partition.hpp"
#include "Processing.hpp"


class DifferentialGrowth{
	struct Node{
		Node* next;
		Node* prev;
		vec2 pos;
		int age;
	};

	public:

		~DifferentialGrowth(){
			clearNodes();
		}
		inline vec2 keepNear(const Node * a, const Node * b);
		inline vec2 repel(const Node * a, const Node * b);
		bool imSettings();

		void render(Processing* ctx);
		void update();
	private:
	void generateNodes();
	void clearNodes(){
		for(auto& n : nodes){
			delete(n);
		}
		nodes.clear();
	}
	vector<Node*> nodes;
	Partition<Node*> p;
	int numNodes = 520;
	float attractForce = .3f;
  float edgeForce = .02f;
	float radius = .001f;
	float desiredDist = .00831f;
	float repelForce = .0045f;
	float repelDist = 0.04f;
  float startRadius = 0.259f;
  float addChance = 0.93f;
  int gammaGain = 10;
  bool DrawPoints = false;
};

#endif