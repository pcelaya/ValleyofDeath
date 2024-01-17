#ifndef __LEVEL1_H__
#define __LEVELÑ1_H__

#include "Module.h"
#include "Player.h"
#include "Ghost.h"

struct SDL_Texture;

struct limit
{
	int left;
	int right;
};

class Level1 : public Module
{
public:

	Level1();

	// Destructor
	virtual ~Level1();

	// Called before render is available
	bool Awake(pugi::xml_node config);

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

public:

	float textPosX, textPosY = 0;
	uint texW, texH;
	uint windowW, windowH;
	limit limitCamera;

	// Debug mode
	bool debug;

	Player* player;
};

#endif // __LEVEL1_H__