//
// Created by matt on 8/20/18.
//

#include "sketches.hpp"

#include "ref/FastNoise/FastNoise.h"
#include "imgui.h"
#include "Random.hpp"
#include "RandomCache.hpp"
using namespace std;

template<typename T>
vector<T> chaikin(vector<T> init, float smooth, float minDist){
  // http://graphics.cs.ucdavis.edu/education/CAGDNotes/Chaikins-Algorithm/Chaikins-Algorithm.html
  vector<T> seed = init;
  int maxIterations = 9; // todo: better heuristic, this will be ~ sizeof(init) * 512
  if(seed.size() == 0){ return {};}
  do {
    vector<T> output{ seed[0] };
    bool needed_cut = false;
    for(int i=0; i < seed.size() - 1; i++){
      if(glm::distance(seed[i], seed[i+1]) < minDist) { //i think it's fucky here?
        output.push_back( seed[i] );
        continue;
      };
      needed_cut = true;
      T q = glm::mix(seed[i], seed[i+1], smooth);
      T r = glm::mix(seed[i], seed[i+1], 1 - smooth);
      output.push_back( q );
      output.push_back( r );
    }
    if(!needed_cut){
      return output;
    }
    seed = output;
  } while(maxIterations-- > 0);
  return seed;
}

void test_chaikin(){
  vector<vector<float>> params{
      {0, 16},
      {0,1,16},
      {0,1,16, 16},
      {0,0,1,4,7,9,11,3,444,16,16},
  };

  for(auto line : params){
    auto chain = chaikin(line, .25, 1);
    for(auto c : chain){
      cout<< c<<" ";
    }
    cout<<endl;
  }
}

void do_curve(Processing* ctx, bool &_r, bool &_c, int curframe, int maxFrames){
  static int seed = 0;
  static int numPts = 5;
  static float minDist = .1;
  static float thickness = 0.002f;
  static int curLayer = 0;
  static int numLayers = 16;
  static float stepSize = .0003;
  static float noiseSize = 3;
  static int numSamples = 24;
  static float smoothing = .25;
  static float radius = 0.3;
  static FastNoise fnoise;

  static vector<vec3> line;

  bool redraw = false;

  STATIC_DO_ONCE(redraw = true;);

  // Typically we would use ImVec2(-1.0f,0.0f) to use all available width, or ImVec2(width,0.0f) for a specified width. ImVec2(0.0f,0.0f) uses ItemWidth.
  ImGui::ProgressBar((float) curLayer / numLayers, ImVec2(0.0f,0.0f));
  ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
  ImGui::Text("Progress Bar");
  redraw |= ImGui::InputInt("Seed", &seed);
  redraw |= ImGui::SliderFloat("radius", &radius, 0,1);
  redraw |= ImGui::SliderInt("numPts", &numPts, 2, 150);
  redraw |= ImGui::SliderFloat("minDist", &minDist, 0.001f, 2.f);
  redraw |= ImGui::SliderFloat("thickness", &thickness, 0.000001f, .1f);
  redraw |= ImGui::SliderFloat("sliderFloat", &smoothing, 0.01, 1.f);
  redraw |= ImGui::InputFloat("Step Size", &stepSize, 0.0001, 0.1);
  redraw |= ImGui::SliderFloat("NoiseSize", &noiseSize, 0, 1114);
  redraw |= ImGui::SliderInt("NumSamples", &numSamples, 2, 200);
  ImGui::SliderInt("Layers", &numLayers, 2, 400);

  if(!redraw && curLayer >= numLayers )
  {
    redraw = false;
    _r = false;
    _c = false;
    return;
  }

  curLayer++;
  if(redraw) {
    curLayer = 0;
  }
  ctx->clear();
  Random::seed(seed + curLayer);
  fnoise.SetSeed(seed+curLayer);

  if(redraw){
    line.clear();
    //Generate base line
    float di = 6.28f / (numPts - 1) ;
    for (int i = 0; i < numPts; i++) {
      vec3 newpt{sin(i*di) * radius, cos(i * di) * radius, 0};
      line.push_back(newpt);
    }
  }
  // moveDots
  for(int sample = 0; sample < numSamples; sample++) {
    for (int i = 0; i < line.size(); i++) {
      line[i].x +=  fnoise.GetNoise(line[i].x * noiseSize, line[i].y * noiseSize, (float) curLayer / numLayers * noiseSize) *         stepSize ;
      line[i].y +=  fnoise.GetNoise(line[i].x * noiseSize, line[i].y * noiseSize, 1290 + (float) curLayer / numLayers * noiseSize) *         stepSize ;
    }

    vector<vec3> pts = chaikin(line, smoothing, minDist);
    ctx->spline(pts, {1, 1, 1, 1}, {0, 0, 0, 0}, thickness);
  }
  ctx->flush();
  _r = true;
  _c = curLayer == 0;
  return ;
}

void spawnFlower(Processing *ctx) {

  const float pi = 3.1415f;

  static vec2 frequency{5, .1f};
  static float magnitude = 2;
  static int samples = 64;
  static int flowerSeed = 0;
  static float fullscale = 1;
  static float decay = .1f;
  static vec4 startColor{1, 0, 0, 0};
  static vec4 endColor{1, 1, 0, 1};

  bool redraw = false;
  redraw |= ImGui::InputInt("Seed", &flowerSeed);
  redraw |= ImGui::SliderFloat2("Frequency", (float *)&frequency, 1.f, 400.f);
  redraw |= ImGui::SliderFloat("Magnitude", &magnitude, 0.f, 10.f);
  redraw |= ImGui::SliderInt("Samples", &samples, 4, 256);
  redraw |= ImGui::SliderFloat("Decay", &decay, .01f, 1.f);
  redraw |= ImGui::SliderFloat("Fullscale", &fullscale, .001f, 2);
  if( ! redraw) return;
  ctx->clear();
  FastNoise noise;
  noise.SetSeed(flowerSeed);

  auto noisePoint = [=](float i, float theta) {
    vec3 pos{sin(theta), cos(theta),0};
    float r = i + decay * magnitude * (.5 - noise.GetNoise(pos.x * frequency[0] , pos.y * frequency[0], i * frequency[1]));
    return pos * r * fullscale;
  };
  auto noiseColor = [=](float i, float theta) {
    return mix(startColor, endColor, abs(sin(theta/2)+i));
  };

  float dTheta = 2 * pi / samples;
  UI_Vertex center{{0, 0, 0}, {0,0,0,0}};
  for (float i = 1; i > .001f; i -= decay) {
    for (float theta = 0; theta < (2 * pi); theta += dTheta) {
      auto a = noisePoint(i, theta);
      auto b = noisePoint(i, theta + dTheta);
      ctx->tri(UI_Vertex{a, noiseColor(i, theta)},
               UI_Vertex{b, noiseColor(i, theta + dTheta)}, center);
    }
  }
  ctx->flush();
}


void drawPendulum(Processing* ctx, bool& redraw, bool& clear, int curFrame, int maxFrames){
  redraw = true;
  clear = false;
  STATIC_DO_ONCE(clear = true;);

  static float time = 0;
  static float deltaTime = .001;
  static int numIterations = 2;

  static float dAngle = .0001;
  static float ratio = 1;
  static float radius_ratio = 1;
  static int cutoff = 0;
  ImGui::SliderInt("num interations", &numIterations, 1, 1000);
  clear |= ImGui::SliderInt("cutoff", &cutoff, 0, 10);
  clear |= ImGui::DragFloat("angleSpeed", &dAngle, .001, 0.01);
  clear |= ImGui::DragFloat("ratio", &ratio, .001, 21);
  clear |= ImGui::DragFloat("radius_ratio", &radius_ratio, .001, 1);
  clear |= ImGui::DragFloat("DeltaTime", &deltaTime, 0.001, 1);

  static float curAngle = 0;
  for(int iter = 0; iter < numIterations; iter++) {
    curAngle += dAngle * .1;
    time += deltaTime;
    vec3 cur{0,0,0};
    vector<vec3> points{};
    for (int i = 0; i < 10; i++) {
      //float nextAngle = curAngle / (i + 1);
      float nextAngle = curAngle  * pow(ratio, i);
      if(i%2 ==0) nextAngle *= -1;
      vec3 next{sin(nextAngle), cos(nextAngle),0};
      next = (next * ((10 - i) * radius_ratio)/10) + cur;
      if(i > cutoff) {
        points.push_back(next);
        ctx->ngon(next, .004,8, vec4{1,1,1,0}, vec4{1,1,1,1} );
      }
      cur = next;
//      ctx->line(cur, next);
    }
    float r = abs(cos(time));
   // ctx->spline(points, vec4{r,.1, .5, 1}, vec4{r,.1,.5, .0}, .001);
  }
}


void drawNoise(Processing* ctx, bool& redraw, bool& clear, int curFrame, int maxFrames){
  STATIC_DO_ONCE(clear = true);
  static FastNoise n;
  static RandomCache rx{1000, 2, [](vec2 pos){return n.GetValueFractal(pos.x, pos.y, 0);}};
  static RandomCache ry{1000, 2, [](vec2 pos){return n.GetValueFractal(pos.x, pos.y, 100);}};
  static float noisefreq = 8.31511;
  static int numpts = 900;
  static float maxVel = .005;
  static vector<vec4> pts;
  static float angleVel = 1.451;
  static float noiseScale = .2f;
  static int numIterations = 1;
  static int curIterations = 0;
  static int totalIterations = 1000;
  ImGui::ProgressBar((float) curIterations / totalIterations, ImVec2(0.0f,0.0f));
  ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
  ImGui::Text("frame progress");
  bool reNoise = ImGui::SliderFloat("frequency", &noisefreq, .01, 15);
  STATIC_DO_ONCE(reNoise = true);
  reNoise|= ImGui::SliderFloat("timeScale", &noiseScale, 0.01, 1);

  //reNoise = true;
  if(reNoise || curIterations >= totalIterations){
    //totalTime += .01;
    const float totalTime = (float) 6.28f * curFrame / maxFrames;
    const float sT = sin(totalTime)*noiseScale;
    const float cT = cos(totalTime)*noiseScale;
    n.SetFrequency(noisefreq);

    rx.reseed(1, [=](vec2 pos){return n.GetValueFractal(pos.x, pos.y + sT, cT);});
    ry.reseed(1, [=](vec2 pos){return n.GetValueFractal(pos.x, pos.y + sT,100+ cT);});
    clear = true;
  }
  ImGui::LabelText("cur iterations", "cur iterations %d", curIterations);
  clear |= ImGui::SliderInt("numPts", &numpts, 10, 5000);
  clear |= ImGui::SliderFloat("velocity", &maxVel, 0,.1);
  clear |= ImGui::SliderFloat("angleVel", &angleVel, 0,20);
  ImGui::SliderInt("num iterations", &numIterations, 1, 100);
  ImGui::SliderInt("total Iterations" , &totalIterations, 100, 100000);

  if(clear){
    pts.clear();
    curIterations= 0;
  }
  redraw = true;
  curIterations += numIterations;
  for(int iter = 0; iter < numIterations; iter++) {
    for (int i = 0; i < numpts; i++) {

      /*
      auto pt = vec2(pts[i].x, pts[i].y);
      auto vel = vec2(pts[i].z, pts[i].w);
      float noisePt = r.sample((pt * .5) + vec2(.5, .5));

      pts[i].x += cos(vel.x) * maxVel*.1;
      pts[i].y += sin(vel.x) * maxVel*.1;
      pts[i].z = (noisePt)* angleVel * glm::length(pt);
      */

      float ex = Random::nextGaussian() * .5f;
      float wy = Random::nextGaussian() * .5f;
      float radius = angleVel * sqrt(ex*ex +wy*wy);
      vec2 pos {ex,wy};
      pos = pos * .5 + vec2{.5,.5};
      pos = radius * vec2{rx.sample(pos), ry.sample(pos)};
      vec4 color = lerp(vec4{.5,.9, 0, 1}, vec4{.2,.6,.1,1}, abs(ex));
      color.x =abs(ex);
      color.y =abs(wy);
      ctx->ngon(pos, maxVel, 6, vec4{SPLAT3(color), 0}, color);
    }
  }
}

void drawLight(Processing* ctx, bool& redraw, bool& clear, int curFrame, int maxFrames){
  static float angle = 0;
  static float spread = .2;
  static int numrays = 500;
  static int seed =0;
  static int numMirrors = 32;
  static float minDist = .2;
  static float thickness = 0.001;
static vector<vec2> mirrorPts;

STATIC_DO_ONCE(clear = true;);
  clear |= ImGui::InputInt("seed", &seed);
  clear |= ImGui::SliderFloat("spread", &spread, 0, 6.28);
  clear |= ImGui::SliderFloat("angle", &angle, 0, 6.28);
  clear |= ImGui::SliderInt("numrays", &numrays, 0, 10240);
  clear |= ImGui::SliderInt("numMirrors", &numMirrors, 2, 19);
  clear |= ImGui::SliderFloat("minDist", &minDist, 0, 1);

  if(clear){
  Random::seed(seed);
  mirrorPts.clear();
  for(int i=0; i < numMirrors; i++){
    mirrorPts.push_back(Random::gaussPoint() * .4f) ;
  }

  std::sort(mirrorPts.begin(), mirrorPts.end(), [](vec2 a, vec2 b){ return glm::length(a) < glm::length(b);});
}
/*
// Connect close points
    for(int mirror = 0; mirror < mirrorPts.size() - 1; mirror+=2){
      auto pta = mirrorPts[mirror];
      for(int b = mirror+1; b < mirrorPts.size() - 1; b++){
        auto ptb = mirrorPts[b];
        if(glm::distance(pta, ptb) < minDist){
       ((ProcessingGL*) ctx)->line(vec3{SPLAT2(pta),0}, vec3{SPLAT2(ptb), 0}, {1,.4,0,numrays}, thickness);
      }
    }
  }
  */
   for(int mirror = 0; mirror < mirrorPts.size() - 1; mirror+=2){
      auto pta = mirrorPts[mirror];
      auto ptb = mirrorPts[mirror + 1];  
      ((ProcessingGL*) ctx)->line(vec3{SPLAT2(pta),0}, vec3{SPLAT2(ptb), 0}, {1,.4,0,numrays}, thickness);
    
    }

    float fudge =  Random::nextGaussian() * .001;
    float di = 1.0f / numrays;
  for(int i=0; i < numrays; i++){

    float min_dist = 1000;
    Geo::ray minray;
    float rayAngle = fudge+  angle +  i / (numrays * spread);
    vec2 raydir{sin(rayAngle), cos(rayAngle)};
    
    for(int mirror = 0; mirror < mirrorPts.size() - 1; mirror+=2){
      auto r = Geo::rayBounce({{0,0}, raydir}, mirrorPts[mirror], mirrorPts[mirror+1]);
      if(std::get<0>(r) < min_dist){
        min_dist = std::get<0>(r);
        minray = std::get<1>(r);
      }
    }
    if(min_dist < 1000){
       auto newdir = minray.p + normalize(minray.d);
       ((ProcessingGL*) ctx)->line({minray.p.x, minray.p.y, i * di}, {newdir.x, newdir.y, i * di}, {0, 1, 0, 1}, thickness);
       ((ProcessingGL*) ctx)->line(vec3{0,0,0}, {SPLAT2(minray.p), i * di}, {0,0,1,1}, 0.001);
    }
    else{
       ((ProcessingGL*) ctx)->line(vec3{0,0,0}, 20 * vec3(SPLAT2(raydir), i * di), {1,0,0,1}, thickness);
    } 
  }
  redraw = true;
}

/*
// geometryshaded  particles
// TODO: allow procFunction's ctx to be of whatever type part_ctx is 
void drawParticles3(Processing* ctx, bool& redraw, bool& clear, int curFrame, int maxFrames){
  part_ctx->clear();
  static vec3 pos{0,0,0};
  static float size = .03;
  static float texsize = .4;
  static int numpts = 0;
  static vec3 vel{.2,.1, 0};
  ImGui::DragFloat3("Position", (float*)&pos, 0, 1);
  ImGui::DragFloat3("vel", (float*)&vel, 0, 0.1);
  ImGui::DragFloat("size", &size, 0,1);
  ImGui::DragFloat("texsize", &texsize, 0,1);
  ImGui::DragInt("numpts", &numpts, 1, 400);

  Particle_Vertex::particle_data dat = {{.5,.5}, texsize, size};

  for(int i=0; i < numpts; i++){
  vec3 spewpos = Geo::fibonacci(i, numpts);
  part_ctx->indexVert({pos + spewpos, dat});
  }
  m_tonemap.use(GL_TEXTURE0);
}
*/
