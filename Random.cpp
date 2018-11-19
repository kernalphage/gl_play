#include "Random.hpp"

std::default_random_engine Random::generator;

std::uniform_real_distribution<float> Random::distribution;

std::normal_distribution<float> Random::gauss_dist;