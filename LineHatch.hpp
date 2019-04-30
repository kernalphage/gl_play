#include "Definitions.hpp"
#include "Partition.hpp"

struct Line{
    vec2 a, b;

    static float triSign(vec2 a, vec2 b, vec2 c){
        return (a.x - c.x) * (b.y - c.y) - (a.y - c.y) * (b.x - c.x);
    }
    // optional of float,vec2? I'll play it by ear
    float intersect(Line other){
        vec2 c,d;
        c = other.a;
        d = other.b;
        float a1 = Line::triSign(a, b, d);
        float a2 = Line::triSign(a, b, c);
        if(a1 * a2 < 0.0f) {
            float a3 = triSign(c, d, a);
            float a4 = a3 + a2 - a1;
            if(a3 * a4 < 0.0){
                float t = a3 / (a3 - a4);
                vec2 p = a + t * (b - a);
                return t;
            }
        }
        return -1;
    }
};

class HatchField {
    vector<Line*> lines;
    Partition<Line*> p;
};