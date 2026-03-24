#ifndef SPRITES_H
#define SPRITES_H

#include <SDL3/SDL.h>
#include <structs.h>
#include "renderer.h"

typedef struct Frame{
	Texture* texture;
	SDL_Rect source;
	SDL_Rect dest;
} Frame;
typedef struct Sprite{
	Uint16 id;
	SDL_Point offset;
	Frame* frames;
	Uint16 frameCount;
	float fps;
} Sprite;

typedef enum TileFlags{
	TILE_INDEX = 0x0FFF,
	TILE_FLIPX = 0x1000,
	TILE_FLIPY = 0x2000,
	TILE_ROTATE = 0x4000, //rotate by 90 degrees
	TILE_PRIORITY = 0x8000, //drawn over objects
} TileFlags;
typedef struct Tileset{
	Texture* texture;
	Uint16 columns;
	SDL_Point tileSize;
} Tileset;
typedef struct Tilemap{
	Tileset* tileset;
	SDL_Point scale;
	Uint16* tiles;
} Tilemap;

void drawSprite(Texture* target, Sprite* spr, Uint16 frame, SDL_Point pos, bool flipX, bool flipY);

void drawTilemap(Texture* target, Tilemap* tilemap, SDL_Point pos, SDL_Rect area, bool priority);

#endif

