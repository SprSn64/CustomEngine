#ifndef INSTANCES_H
#define INSTANCES_H

#include <SDL3/SDL.h>
#include <structs.h>

typedef struct Instance Instance;
typedef struct Behaviour{
	Uint32 id;

	void (*init)(Instance*);
	void (*update)(Instance*);
	void (*draw)(Instance*);
	void (*remove)(Instance*);
} Behaviour;

typedef enum InstanceFlags{
	INSTANCE_ACTIVE = 0x00000001,
	INSTANCE_VISIBLE = 0x00000002,
	INSTANCE_FLIPX = 0x00000004,
	INSTANCE_FLIPY = 0x00000008,
	INSTANCE_BASEFLAGS = 0x00000003
} InstanceFlags;

typedef struct Sprite Sprite;
typedef struct Instance{
	Behaviour* bhv;
	Uint32 flags;
	void** items;

	Sprite* spr;
	float frame;
	SDL_FPoint pos;

	struct Instance* prev;
	struct Instance* next;
} Instance;

Instance* newInstance(Behaviour* bhv, float posX, float posY, Sprite* spr, Uint32 flags);
void removeInstance(Instance* item);

void updateInstance(Instance* item);
void drawInstance(Instance* item);

void updateInstances(); 
void drawInstances();

void drawSelf(Instance* item);

#endif
