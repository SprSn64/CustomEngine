#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <structs.h>
#include "math.h"
#include "renderer.h"
#include "sprites.h"
#include "instances.h"
#include "behaviours.h"

SDL_Point screenRes = {320, 240};
Texture* displayTex = NULL;
SDL_Point windowSize = {640, 480};

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture *renderTex = NULL;

float renderScale = 2;

Uint64 last = 0;
Uint64 now = 0;
double deltaTime = 0;
Uint32 lastFPS = 0;
char* fpsText;
float guiReset = 0;

double timer = 0;
SDL_Point cameraPos = {0, 0};

Texture* plueyTex = NULL;
Texture* testTex = NULL;
Texture* tilesetTex = NULL;

Texture* playerTex = NULL;
Texture* fontTex = NULL;

Font baseFont;

//Sprite testSprite = {1, (SDL_Point){-16, -32}, NULL, 4, 8};
//Sprite plueySpr = {2, (SDL_Point){-12, -12}, NULL, 1, 1};

Sprite* testSprite = NULL;
Sprite* plueySpr = NULL;

Tileset testTileset = {NULL, 8, (SDL_Point){16, 16}};
Tilemap testTilemap = {&testTileset, (SDL_Point){8, 8}, NULL};

ButtonMap keyList[KEYBIND_MAX];
void HandleKeyInput();

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]){
	(void)appstate; (void)argc; (void)argv;

	window = SDL_CreateWindow("Object Engine", windowSize.x, windowSize.y, SDL_WINDOW_RESIZABLE);
	if(!window){
		SDL_Log("Couldn't create window: %s", SDL_GetError());
		return SDL_APP_FAILURE;
	}
	SDL_SetWindowMinimumSize(window, screenRes.x, screenRes.y);

	renderer = SDL_CreateRenderer(window, NULL); SDL_SetRenderVSync(renderer, 1);
	renderTex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_XBGR8888, SDL_TEXTUREACCESS_TARGET, screenRes.x, screenRes.y);
	SDL_SetTextureScaleMode(renderTex, SDL_SCALEMODE_PIXELART);
	SDL_SetRenderVSync(renderer, 1);

	displayTex = newTexture(320, 240);

	//plueyTex = loadTexture("assets/pluey.png");
	testTex = loadTexture("assets/spamton.png");
	tilesetTex = loadTexture("assets/tiles.png");

	//playerTex = loadTexture("assets/player.png");
	fontTex = loadTexture("assets/font.png");

	addSpriteSheet("assets/player.png", 1);
	addSpriteSheet("assets/pluey.png", 2);

	baseFont = (Font){fontTex, 32, (SDL_Point){8, 8}, (SDL_Point){8, 8}, (SDL_FPoint){7, 0}, 16};
	fpsText = malloc(12);

	testTileset.texture = tilesetTex;
	testTilemap.tiles = malloc(sizeof(Uint16) * testTilemap.scale.x * testTilemap.scale.y);
	for(int i=0; i<testTilemap.scale.x * testTilemap.scale.y; i++){
		testTilemap.tiles[i] = i + ((i + i/testTilemap.scale.x) & 1) * TILE_PRIORITY;
	}

	keyList[KEYBIND_UP].code = SDL_SCANCODE_UP; keyList[KEYBIND_DOWN].code = SDL_SCANCODE_DOWN; keyList[KEYBIND_LEFT].code = SDL_SCANCODE_LEFT; keyList[KEYBIND_RIGHT].code = SDL_SCANCODE_RIGHT; 
	keyList[KEYBIND_A].code = SDL_SCANCODE_X; keyList[KEYBIND_B].code = SDL_SCANCODE_Z; keyList[KEYBIND_START].code = SDL_SCANCODE_RETURN; keyList[KEYBIND_ESC].code = SDL_SCANCODE_ESCAPE; 

	/*testSprite.frames = malloc(sizeof(Frame) * 4); 
	testSprite.frames[0].texture = playerTex; testSprite.frames[0].source = (SDL_Rect){0, 0, 32, 32}; testSprite.frames[0].dest = (SDL_Rect){0, 0, 32, 32};
	testSprite.frames[1].texture = playerTex; testSprite.frames[1].source = (SDL_Rect){32, 0, 32, 32}; testSprite.frames[1].dest = (SDL_Rect){0, 0, 32, 32};
	testSprite.frames[2].texture = playerTex; testSprite.frames[2].source = (SDL_Rect){0, 0, 32, 32}; testSprite.frames[2].dest = (SDL_Rect){0, 0, 32, 32};
	testSprite.frames[3].texture = playerTex; testSprite.frames[3].source = (SDL_Rect){0, 32, 32, 32}; testSprite.frames[3].dest = (SDL_Rect){0, 0, 32, 32};*/
	testSprite = loadSpritePack("assets/spritepacks/player.spk");

	//plueySpr.frames = malloc(sizeof(Frame)); 
	//plueySpr.frames[0].texture = plueyTex; plueySpr.frames[0].source = (SDL_Rect){0, 0, 24, 24}; plueySpr.frames[0].dest = (SDL_Rect){0, 0, 24, 24};
	plueySpr = loadSpritePack("assets/spritepacks/pluey.spk");

	Instance* solidItem = newInstance(&solidBhv, -1024, 0, NULL, INSTANCE_BASEFLAGS); SDL_FRect* solidScale = &solidItem->items[0];
	solidScale->x = 2048; solidScale->y = 32;

	newInstance(&playerBhv, 0, 0, testSprite, INSTANCE_BASEFLAGS);
	newInstance(&plueyBhv, 200, 120, plueySpr, INSTANCE_BASEFLAGS);

	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event){
	(void)appstate;
	if(event->type == SDL_EVENT_QUIT){
		return SDL_APP_SUCCESS;
	}
	
	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate){
	(void)appstate;
	SDL_GetWindowSize(window, &windowSize.x, &windowSize.y);
	renderScale = min((float)windowSize.x / screenRes.x, (float)windowSize.y / screenRes.y);

	SDL_RenderClear(renderer);

	last = now;
	now = SDL_GetTicks();
	deltaTime = min(((double)now - (double)last) / 1000.0f, 1);
	timer += deltaTime;

	HandleKeyInput();
	updateInstances();

	SDL_Point tilePos = {
		-((cameraPos.x + (displayTex->width >> 1)) % 16), 
		-((cameraPos.y + (displayTex->height >> 1)) % 16)
	};
	SDL_Rect tileArea = {(cameraPos.x + (displayTex->width >> 1)) / 16, (cameraPos.y + (displayTex->height >> 1)) / 16, 22, 16};

	clearTex(displayTex, 0xff808080);
	drawTilemap(displayTex, &testTilemap, tilePos, tileArea, false);
	drawInstances();
	drawTilemap(displayTex, &testTilemap, tilePos, tileArea, true);

	/*drawRect(displayTex, 160 + cos(timer) * 160, 120 + sin(timer) * 120, 32, 32, 0xFF0000FF);
	drawTexture(displayTex, plueyTex, &(SDL_Rect){0, 6, 12, 12}, &(SDL_Rect){148 + sin(timer) * 148, 108 + cos(timer) * 108, 24, 24}, 0xFFFFFFFF);
	drawTexture(displayTex, plueyTex, &(SDL_Rect){0, 0, 24, 24}, &(SDL_Rect){120, 160, 24 + sin(timer) * 24, 24 + sin(timer) * 24}, 0xFFFFFFFF);

	drawTexture(displayTex, testTex, &(SDL_Rect){0, 0, testTex->width, testTex->height}, &(SDL_Rect){
		160 - (testTex->width>>1) + sin(timer * 12) * sin(timer / 4) * 12, 240 - testTex->height, 
		testTex->width, testTex->height}, 0x80FFFFFF);*/

	drawHamLine(displayTex, (SDL_Point){160, 120}, (SDL_Point){sin(timer) * 160 + 160, cos(timer) * 120 + 120}, 0xFF000000);

	guiReset -= deltaTime;
	if(guiReset <= 0){
		guiReset = 0.06;
		sprintf(fpsText, "FPS: %d", (int)(1/deltaTime));
	}
	drawText(displayTex, &baseFont, fpsText, 0, 0, 1, 0xFFFFFFFF);

	SDL_UpdateTexture(renderTex, NULL, displayTex->pixels, displayTex->width * 4);
	SDL_RenderTexture(renderer, renderTex, &(SDL_FRect){0, 0, displayTex->width, displayTex->height}, 
		&(SDL_FRect){
			(int)(windowSize.x - (displayTex->width * renderScale)) >> 1, 
			(int)(windowSize.y - (displayTex->height * renderScale)) >> 1, 
			displayTex->width * renderScale, 
			displayTex->height * renderScale
		}
	);
	SDL_RenderPresent(renderer);

	return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result){
	(void)appstate; (void)result;
}

void HandleKeyInput(){
	const bool* keyState = SDL_GetKeyboardState(NULL);
	bool hasFocus = SDL_GetWindowFlags(window) & SDL_WINDOW_INPUT_FOCUS;
	for(int i = 0; i < KEYBIND_MAX; i++){
		keyList[i].down = hasFocus && keyState[keyList[i].code];
		keyList[i].pressed = false;
		if(keyList[i].down){
			if(!keyList[i].pressCheck){
				keyList[i].pressCheck = true;
				keyList[i].pressed = true;
			}
		}else keyList[i].pressCheck = false;
	}
}