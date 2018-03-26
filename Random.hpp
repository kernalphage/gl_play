#ifndef RANDOM_HPP
#define RANDOM_HPP
#include <random>

class Random{
	static std::default_random_engine generator;
  	static std::uniform_real_distribution<float> distribution;
  public:
  	static void seed(int s = 0){
  		generator = std::default_random_engine{};
		generator.seed(s);
  		distribution = std::uniform_real_distribution<float>{0.0f, 1.0f};
  	}
  	static float f(){
  		return distribution(generator);
	}
	static int range(int a, int b){
		return (int)( a + (b - a) * distribution(generator));
	}
	static float range(float a, float b){
		return a + (b - a) * distribution(generator);
	}
};

#endif