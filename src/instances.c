#include "instances.h"

#include <SDL3/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <structs.h>
#include "math.h"
#include "renderer.h"
#include "sprites.h"

extern Texture* displayTex;

Instance* headInstance = NULL;

Instance* newInstance(Behaviour* bhv, float posX, float posY, Sprite* spr, Uint32 flags){
	Instance* newItem = malloc(sizeof(Instance));
	if(!newItem) return NULL;

	newItem->bhv = bhv; newItem->spr = spr; newItem->flags = flags;
	newItem->pos = (SDL_FPoint){posX, posY};
	newItem->frame = 0; newItem->prev = NULL; newItem->next = NULL;

	if(!headInstance) headInstance = newItem;
	else{
		Instance* currItem = headInstance;
		while(currItem){
			if(currItem->next){currItem = currItem->next; continue;}
			currItem->next = newItem; newItem->prev = currItem; break;
		}
	}

	if(!bhv) return newItem;
	if(bhv->init) bhv->init(newItem);

	return newItem;
}

void removeInstance(Instance* item){
	if(item->bhv && item->bhv->init) item->bhv->remove(item);

	if(headInstance == item) headInstance = item->next;
	if(item->prev) item->prev->next = item->next;
	if(item->next) item->next->prev = item->prev;
	free(item);
}

extern double deltaTime;
void updateInstance(Instance* item){
	if(item->spr)
		item->frame = fmod(item->frame + item->spr->fps * deltaTime, item->spr->frameCount);

	if(item->bhv && item->bhv->update) item->bhv->update(item);
}
void drawInstance(Instance* item){
	if(item->bhv && item->bhv->draw) item->bhv->draw(item);
	//setPixel(displayTex, item->pos.x, item->pos.y, 0xFF0000FF);
}

void updateInstances(){
	Instance* currItem = headInstance;
	while(currItem){
		if(currItem->flags & INSTANCE_ACTIVE) updateInstance(currItem);
		currItem = currItem->next;
	}
}
void drawInstances(){
	Instance* currItem = headInstance;
	while(currItem){
		if(currItem->flags & INSTANCE_VISIBLE) drawInstance(currItem);
		currItem = currItem->next;
	}
}

extern SDL_Point cameraPos;
void drawSelf(Instance* item){
	if(item->spr) 
		drawSprite(displayTex, item->spr, (Uint16)item->frame, 
			(SDL_Point){
				item->pos.x - cameraPos.x + (displayTex->width >> 1), 
				item->pos.y - cameraPos.y + (displayTex->height >> 1)
			}, 
			item->flags & INSTANCE_FLIPX, item->flags & INSTANCE_FLIPY
		);
}