#ifndef MATH_H
#define MATH_H

#include <SDL3/SDL.h>

bool getBit(int input, Uint8 bit);
int setBit(int input, Uint8 bit, bool set);

float lerp(float a, float b, float t);
bool between(float value, float min, float max);

#endif
