#include "Definitions.hpp"
#include "Partition.hpp"

struct Line{
    vec2 a, b;

    static float triSign(vec2 a, vec2 b, vec2 c){
        return (a.x - c.x) * (b.y - c.y) - (a.y - c.y) * (b.x - c.x);
    }
    static vec2 sample(float t) {
        return lerp(a, b, t);
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
        return 1; // 1 as in, no collision
    }
};

class HatchField {
    vector<Line*> lines;
    Partition<Line*> p;
};


class Hatch {
    
    float minDist(Line* l, const Partition p) {
        vector<Line*> cur_neighbors;
        p.neighbors(l, std::back_inserter(cur_neighbors));
        float minT = 1;

        for(auto& n : cur_neighbors){
            minT = min(minT, l->intersect(*n));
        }
        return minT;
    }

    float cropLine(Line* l, const Partition& p){
        float minT = minDist(l, p);
        l->b = l->sample(minT);
    }

    std::vector<Line*> drawPatch(vec2 origin, float rotation, float scale, int numHatches){
        const vec2 forward = + a2v(rotation) * scale;
        const vec2 perp{-forward.y, forward.x};  // TODO: jiggle perp per line
        const float di = 1.0f / numHatches;

        Line north{origin,  origin + forward};
        Line south{origin,  origin - forward};

        float maxN = minDist(north, p);
        float maxS = minDist(north, p);
        
        vector<Line*> outLines;
        for(int i=0; i < numHatches; i++){
            if(i*di < maxN){
                vec2 origN = north.sample(i * di);
                Line* eastN= new Line{origN, origN+perp};
                Line* westN= new Line{origN, origN-perp};
                cropLine(eastN);
                cropLine(westN);
                outLines.push_back(new Line{eastN->b, westN->b});
                p.add(outLines.back())
            }
            if(i*di < maxS){
                vec2 origS = south.sample(i * di);
                Line* eastS= new Line{origS, origS+perp};
                Line* westS= new Line{origS, origS-perp};
                cropLine(eastS);
                cropLine(westS);
                outLines.push_back(new Line{eastS->b, westS->b});
                p.add(outLines.back())
            }
        }

        return outLines;
    };


    struct Segment {
        Line l;
        int depth;
    };

    std::vector<Line*> drawSegment(){
        // lightning bolt subdivisions 
        vec2 centerTop = vec2(0, Random::f());
        vec2 centerBottom = vec2(1, Random::f());
        vector<Line*> lines;
        vector<int> depths;

        lines.push_back({centerBottom, centerTop}, Random.range(subdivisions));

    };

    float curLoc(int i, int maxI){
        return Random::range(((float) i) /maxI);
    }
    void drawBolt() {
        vector<Line*> segments = drawSegment();

        for(int i=0; i < numPatches; i++){
            vec2 origin = segments[0].sample(curLoc(i,numPatches));
            vector<Line*> hatches = drawPatch(origin, Random::range(s_HatchRotation), Random::range(s_HatchScale), numHatches );
        }
    }
    void render(Processing* ctx){
        std::vector<Line*> segments = drawBolt();

    };
};