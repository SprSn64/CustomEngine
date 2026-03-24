#ifndef STRUCTS_H
#define STRUCTS_H

#include <SDL3/SDL.h>

#define max(a,b) ((a) > (b) ? (a) : (b))
#define min(a,b) ((a) < (b) ? (a) : (b))

typedef enum keyBinds{
	KEYBIND_UP, KEYBIND_DOWN, KEYBIND_LEFT, KEYBIND_RIGHT,
	KEYBIND_A, KEYBIND_B, KEYBIND_START, KEYBIND_ESC,    
	KEYBIND_MAX
} keyBinds;
typedef struct{
	bool down, pressed, released, pressCheck;
	Uint32 code;
} ButtonMap;

#endif
