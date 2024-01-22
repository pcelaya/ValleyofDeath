#ifndef __INTRO_H__
#define __INTRO_H__

#include "Module.h"

struct SDL_Texture;

class Intro : public Module
{
public:

	Intro();

	// Destructor
	virtual ~Intro();

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
	pugi::xml_node config;
	uint texW, texH;
	uint windowW, windowH;

	SDL_Texture* gameLogo;

};

#endif // __INTRO_H__