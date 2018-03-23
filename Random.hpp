#include <random>

class Random{
	static std::default_random_engine generator;
  	static std::uniform_real_distribution<float> distribution;
  public:
  	static void seed(){
  		generator = std::default_random_engine{};
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