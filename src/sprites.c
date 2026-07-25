#include "sprites.h"

#include <SDL3/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <structs.h>
#include "renderer.h"
#include "math.h"

SpriteSheet* headSprSheet = NULL;

SpriteSheet* addSpriteSheet(char* path, Uint16 id){
	SpriteSheet* newSheet = malloc(sizeof(SpriteSheet));
	if(!newSheet) return NULL;

	newSheet->texture = loadTexture(path);
	newSheet->id = id;

	if(!headSprSheet){
		headSprSheet = newSheet; 
		return newSheet;
	}

	SpriteSheet* currSprSheet = headSprSheet;
	while(currSprSheet->next){
		currSprSheet = currSprSheet->next;
	}
	currSprSheet->next = newSheet;
	newSheet->prev = currSprSheet;

	return newSheet;
}
SpriteSheet* getSpriteSheet(Uint16 id){
	SpriteSheet* currSprSheet = headSprSheet;
	while(currSprSheet){
		if(currSprSheet->id == id) return currSprSheet;
		currSprSheet = currSprSheet->next;
	}

	return NULL;
}

void drawSprite(Texture* target, Sprite* spr, Uint16 frame, SDL_Point pos, bool flipX, bool flipY){
	if(!target || !spr) return;

	Frame* currFrame = &spr->frames[frame % spr->frameCount];
	SDL_Rect newDest = {currFrame->dest.x * (1 - 2 * flipX) + pos.x + spr->offset.x, currFrame->dest.y * (1 - 2 * flipY) + pos.y + spr->offset.y, currFrame->dest.w * (1 - 2 * flipX), currFrame->dest.h * (1 - 2 * flipY)};

	drawTexture(target, currFrame->sheet->texture, &currFrame->source, &newDest, WHITE);
}

/*void drawSpriteAdv(Texture* target, Sprite* spr, Uint16 frame, SDL_Rect* dest, Uint32 colour){
	if(!target || !spr) return;

	Frame* currFrame = &spr->frames[frame % spr->frameCount];
	SDL_Rect newDest = {currFrame->dest.x + dest->x + spr->offset.x, currFrame->dest.y + dest->x + spr->offset.y, currFrame->dest.w, currFrame->dest.h};

	drawTexture(target, currFrame->texture, &currFrame->source, &newDest, colour);
}*/

Sprite* loadSpritePack(char* path){
	FILE *file = fopen(path, "rb");
	if(!file)
		return NULL;

	Sprite* loadedSprite = NULL;

	fseek(file, 0, SEEK_END);
	Uint32 fileSize = ftell(file) + 1;
	rewind(file);

	Uint8* fileData = malloc(sizeof(Uint8) * fileSize);
	fileData = fgets(fileData, fileSize, file);

	printf("Sprite Pack Data (%d Bytes):\n", fileSize);
	for(Uint32 i=0; i<fileSize; i++){
		printf("%x ", fileData[i]);
	}
	printf("\n");

	if(strncmp(fileData, "SPK", 3)){
		printf("%s is not a sprite pack!\n", path); 
		free(fileData);
		goto spkFileEnd;
	}
	printf("%s is a valid sprite pack!\n", path);

	Uint32 currLoc = 3;
	//while(currLoc < fileSize)
		loadedSprite = loadSpriteData(&fileData[currLoc], &currLoc);

	free(fileData);
spkFileEnd:
	fclose(file);
	return loadedSprite;
}

extern Texture* playerTex;
Sprite* loadSpriteData(Uint8* data, Uint32* loc){
	if(!data) return NULL;

	Sprite* newSprite = malloc(sizeof(Sprite));
	if(!newSprite) return NULL;

	newSprite->id = (Uint16)joinBytes(2, data[1], data[0]);
	newSprite->offset = (SDL_Point){(Sint16)joinBytes(2, data[3], data[2]), (Sint16)joinBytes(2, data[5], data[4])};
	newSprite->frameCount = (Uint16)joinBytes(2, data[7], data[6]);

	Uint32 tempFPS = joinBytes(4, data[11], data[10], data[9], data[8]);
	newSprite->fps = *(float*)&tempFPS;

	printf("Sprite Data:\n");
	for(int i=0; i<12 + 18 * newSprite->frameCount; i++){
		printf("%x ", data[i]);
	}
	printf("\n");

	printf("New sprite id: %d frameCount: %d FPS: %f\n", newSprite->id, newSprite->frameCount, newSprite->fps);
	printf("Offset X: %d, Offset Y: %d\n", newSprite->offset.x, newSprite->offset.y);

	newSprite->frames = malloc(sizeof(Frame) * newSprite->frameCount); 
	for(int i=0; i<newSprite->frameCount; i++){
		int frameLoc = 12 + 18 * i;

		printf("Frame Data:\n");
		for(Uint32 j=0; j<18; j++){
			printf("%x ", data[j + frameLoc]);
		}
		printf("\n");

		Frame* currFrame = &newSprite->frames[i];
		currFrame->sheet = getSpriteSheet((Uint16)joinBytes(2, data[frameLoc + 1], data[frameLoc])); 
		currFrame->source = (SDL_Rect){
			(Uint16)joinBytes(2, data[frameLoc + 3], data[frameLoc + 2]), 
			(Uint16)joinBytes(2, data[frameLoc + 5], data[frameLoc + 4]), 
			(Uint16)joinBytes(2, data[frameLoc + 7], data[frameLoc + 6]), 
			(Uint16)joinBytes(2, data[frameLoc + 9], data[frameLoc + 8])
		}; 
		currFrame->dest = (SDL_Rect){
			(Uint16)joinBytes(2, data[frameLoc + 11], data[frameLoc + 10]), 
			(Uint16)joinBytes(2, data[frameLoc + 13], data[frameLoc + 12]), 
			(Uint16)joinBytes(2, data[frameLoc + 15], data[frameLoc + 14]), 
			(Uint16)joinBytes(2, data[frameLoc + 17], data[frameLoc + 16])
		}; 
	}

	if(loc)
		loc += 12 + 18 * newSprite->frameCount;

	return newSprite;
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