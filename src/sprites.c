#include "sprites.h"

#include <SDL3/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <structs.h>
#include "renderer.h"
#include "math.h"

void drawSprite(Texture* target, Sprite* spr, Uint16 frame, SDL_Point pos, bool flipX, bool flipY){
	if(!target || !spr) return;

	Frame* currFrame = &spr->frames[frame % spr->frameCount];
	SDL_Rect newDest = {currFrame->dest.x * (1 - 2 * flipX) + pos.x + spr->offset.x, currFrame->dest.y * (1 - 2 * flipY) + pos.y + spr->offset.y, currFrame->dest.w * (1 - 2 * flipX), currFrame->dest.h * (1 - 2 * flipY)};

	drawTexture(target, currFrame->texture, &currFrame->source, &newDest, WHITE);
}

/*void drawSpriteAdv(Texture* target, Sprite* spr, Uint16 frame, SDL_Rect* dest, Uint32 colour){
	if(!target || !spr) return;

	Frame* currFrame = &spr->frames[frame % spr->frameCount];
	SDL_Rect newDest = {currFrame->dest.x + dest->x + spr->offset.x, currFrame->dest.y + dest->x + spr->offset.y, currFrame->dest.w, currFrame->dest.h};

	drawTexture(target, currFrame->texture, &currFrame->source, &newDest, colour);
}*/

Sprite* loadSpriteBank(char* path){
	FILE *file = fopen(path, "rb");
	if(!file)
		return NULL;

	fclose(file);
	return NULL;
}

Sprite* loadSpriteData(char* data){
	return NULL;
}

void drawTilemap(Texture* target, Tilemap* tilemap, SDL_Point pos, SDL_Rect area, bool priority){
	Tileset* tileset = tilemap->tileset;

	for(int i=0; i < area.w * area.h; i++){
		SDL_Point tileLoc = {
			((area.x % tilemap->scale.x) + (i % area.w)) % tilemap->scale.x, 
			(area.y + (i / area.w)) % tilemap->scale.y
		};

		Uint16 tileIndex = tileLoc.x + tileLoc.y * tilemap->scale.x;
		Uint16 tileVal = tilemap->tiles[tileIndex] & TILE_INDEX;
		if(tileVal == 0 || (tilemap->tiles[tileIndex] & TILE_PRIORITY) >> 15 == priority) continue;
		drawTexture(target, tileset->texture, 
			&(SDL_Rect){
				(tileVal % tileset->columns) * tileset->tileSize.x, 
				tileVal / tileset->columns * tileset->tileSize.y, 
				tileset->tileSize.x, 
				tileset->tileSize.y
			}, 
			&(SDL_Rect){
				pos.x + (i % area.w) * tileset->tileSize.x - (tileset->tileSize.x * getBit(tilemap->tiles[tileIndex], 12)), 
				pos.y + i / area.w * tileset->tileSize.y - (tileset->tileSize.y * getBit(tilemap->tiles[tileIndex], 13)), 
				tileset->tileSize.x * (1 - 2 * getBit(tilemap->tiles[tileIndex], 12)), 
				tileset->tileSize.y * (1 - 2 * getBit(tilemap->tiles[tileIndex], 13))
			}, 
			WHITE);
	}
}