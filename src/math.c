#include "math.h"

#include <SDL3/SDL.h>
#include <stdio.h>
#include <math.h>

bool getBit(int input, Uint8 bit){
	return (input & (1 << bit)) >> bit;
}

int setBit(int input, Uint8 bit, bool set){
	if(getBit(input, bit) != set)
		return input ^= (1 << bit);
	return input;
}

float lerp(float a, float b, float t){
	return a + t * (b - a);
}
bool between(float value, float min, float max){
	return (value >= min && value <= max); 
}