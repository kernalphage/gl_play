//
// Created by matt on 10/7/17.
//

#include "Partition.hpp"
#include "Definitions.hpp"
#include "LineHatch.hpp"

#include <set>
// Ideas for sampler:
// Lerping between 1/2/4 pixels
// halftone modulation? dithering?

// Expect 
template <>
vector<int> Partition<Line *>::index(Line *l)
{
    // TODO: Have line be in multiple buckets
    float d = distance(l->a, l->b);
    set<int> buckets;

    int numSteps = (int) ceil(d * _scale);
    float di = 1.0f / numSteps;
    for(int i=0; i <= numSteps; i++){
        float t = di * i;
        vec2 pos = lerp(l->a, l->b, t);
        buckets.insert(index(pos));
    }
    return vector<int>(buckets.begin(), buckets.end());
};
