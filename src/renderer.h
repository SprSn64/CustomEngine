#ifndef RENDERER_H
#define RENDERER_H

#include <SDL3/SDL.h>
#include <structs.h>

#define WHITE 0xFFFFFFFF

typedef struct Texture{
	Uint32* pixels;
	Uint16 width, height;
} Texture;

typedef struct{
	Texture* image;
	Uint16 startChar; //32 starts at the space glyth
	SDL_Point glyphSize;
	SDL_Point renderSize;
	SDL_FPoint kerning;
	Uint16 columns;
} Font;

Texture* newTexture(Uint16 width, Uint16 height);
Texture* loadTexture(char* file);
Texture* loadRawTexture(char* file);

Uint32 sdlToColour(SDL_FColor colour);
SDL_FColor colourToSDL(Uint32 colour);

Uint32 colourLerp(Uint32 colA, Uint32 colB, float t);
Uint32 colourMult(Uint32 colA, Uint32 colB);

void setPixel(Texture* target, Uint16 x, Uint16 y, Uint32 colour);
Uint32 getPixel(Texture* target, Uint16 x, Uint16 y);
void clearTex(Texture* target, Uint32 colour);

void drawRect(Texture* target, Uint16 posX, Uint16 posY, Uint16 width, Uint16 height, Uint32 colour);
void drawHamLine(Texture* target, SDL_Point pointA, SDL_Point pointB, Uint32 colour);

void drawTexture(Texture* target, Texture* tex, SDL_Rect* source, SDL_Rect* dest, Uint32 colour);
void drawText(Texture* target, Font *textFont, char* text, short posX, short posY, float scale, Uint32 colour);

#endif
