#ifndef __SCENE_H__
#define __SCENE_H__

#include "Module.h"
#include "Player.h"

struct SDL_Texture;

struct limit
{
	int left;
	int right;
};

class Scene : public Module
{
public:

	Scene() : activeScene(false) {}

	// Destructor
	virtual ~Scene() {}

public:
	bool activeScene;

	pugi::xml_node config;
	uint texW, texH;
	uint windowW, windowH;
	limit limitCamera;

	// Debug mode
	bool debug;
};

#endif // __LEVEL1_H__