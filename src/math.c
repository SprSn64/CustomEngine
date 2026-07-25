#include "math.h"

#include <SDL3/SDL.h>
#include <stdio.h>
#include <math.h>
#include <stdarg.h>

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

Uint64 joinBytes(Uint8 count, ...){
	Uint64 output = 0;

	va_list items;
	va_start(items, count); 
	for(Uint8 i=0; i<count; i++){
		output += va_arg(items, int) << (i << 3);
	}

	va_end(items);

	return output;
}