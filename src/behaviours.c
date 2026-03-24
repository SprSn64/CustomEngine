#include "behaviours.h"

#include <SDL3/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <structs.h>
#include "math.h"
#include "renderer.h"
#include "sprites.h"
#include "instances.h"

extern Texture* displayTex;

extern double deltaTime;
extern double timer;

extern ButtonMap keyList[KEYBIND_MAX];

extern SDL_Point cameraPos;

extern Instance* headInstance;

//ignore all the warnings the game crashes if i try to fix them

void playerInit(Instance* item){
	item->items = malloc(2 * sizeof(void));
	item->items[0] = malloc(sizeof(SDL_FPoint));
	item->items[1] = malloc(sizeof(float));
	SDL_FPoint* vel = &item->items[0];

	vel->x = 0; vel->y = 0;
}
void playerUpdate(Instance* item){
	SDL_FPoint* vel = &item->items[0];
	vel->x = (vel->x + (keyList[KEYBIND_RIGHT].down - keyList[KEYBIND_LEFT].down) * 640 * deltaTime) * 0.92;
	vel->y = vel->y + (480 - (keyList[KEYBIND_A].down && vel->y < 0) * 240) * deltaTime;//(vel->y + (keyList[KEYBIND_DOWN].down - keyList[KEYBIND_UP].down) * 320 * deltaTime) * 0.96;

	float* jumpTime = &item->items[1];

	if(*jumpTime <= 4 && keyList[KEYBIND_A].pressed)
		vel->y = -160;
	*jumpTime += 1;

	if(fabs(vel->x) + fabs(vel->y) >= 4){
		item->pos = (SDL_FPoint){
			item->pos.x + vel->x * deltaTime,
			item->pos.y + vel->y * deltaTime
		};
	}
	if(item->pos.y >= 1024){
		item->pos = (SDL_FPoint){0, 0};
		vel->x = 0; vel->y = 0;
	}

	Instance* currSolid = headInstance;
	while(currSolid){
		if(currSolid->bhv->id != 2)
			goto plrSolidEnd;
		SDL_FPoint* solidScale = &currSolid->items[0];
		if(between(item->pos.x - currSolid->pos.x, -8, solidScale->x + 8) && between(item->pos.y - currSolid->pos.y, 0, solidScale->y + 24)){
			item->pos.y = currSolid->pos.y;
			vel->y = 0;
			*jumpTime = 0;
		}
plrSolidEnd:		
		currSolid = currSolid->next;
	}

	cameraPos = (SDL_Point){item->pos.x, item->pos.y};
	if(keyList[KEYBIND_RIGHT].down ^ keyList[KEYBIND_LEFT].down && *jumpTime <= 4)
		item->flags = setBit(item->flags, 2, keyList[KEYBIND_LEFT].down);
	if(!(keyList[KEYBIND_RIGHT].down ^ keyList[KEYBIND_LEFT].down) && *jumpTime <= 4)
		item->frame = 0.99;
}
void playerRemove(Instance* item){
	free(item->items);
}
Behaviour playerBhv = {1, playerInit, playerUpdate, drawSelf, playerRemove};

void plueyUpdate(Instance* item){
	item->pos = (SDL_FPoint){
		.5 + sin(timer) * 128, 
		.5 + cos(timer) * 128
	};
}
Behaviour plueyBhv = {3, NULL, plueyUpdate, drawSelf, NULL};

void solidInit(Instance* item){
	item->items = malloc(sizeof(SDL_FPoint));
}
void solidDraw(Instance* item){
	SDL_FPoint* scale = &item->items[0];

	drawRect(displayTex, 
		item->pos.x - cameraPos.x + (displayTex->width >> 1),
		item->pos.y - cameraPos.y + (displayTex->height >> 1),
		scale->x, scale->y, 0x800000FF);
}
Behaviour solidBhv = {2, solidInit, NULL, solidDraw, playerRemove};
