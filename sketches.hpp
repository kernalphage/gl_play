//
// Created by matt on 8/20/18.
//

#ifndef GL_PLAY_SKETCHES_HPP
#define GL_PLAY_SKETCHES_HPP

#include "Definitions.hpp"
#include <vector>

template<typename T>
std::vector<T> chaikin(std::vector<T> init, float smooth, float minDist);
void test_chaikin();

void do_curve(Processing* ctx, bool &_r, bool &_c, int curframe, int maxFrames);

void spawnFlower(Processing *ctx);

void drawPendulum(Processing* ctx, bool& redraw, bool& clear, int curFrame, int maxFrames);

void drawNoise(Processing* ctx, bool& redraw, bool& clear, int curFrame, int maxFrames);

void drawLight(Processing* ctx, bool& redraw, bool& clear, int curFrame, int maxFrames);

#endif //GL_PLAY_SKETCHES_HPP

