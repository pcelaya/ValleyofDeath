#ifndef __FADETOBLACK_H__
#define __FADETOBLACK_H__

#include "Module.h"

#include "SDL/include/SDL_rect.h"

enum class Fade_Step
{
	NO,
	TO_BLACK,
	FROM_BLACK
};

enum class TypeScene
{
	INTRO,
	MENU,
	LEVEL_1,
	LEVEL_2,
	END,
	NONE
};

class FadeToBlack : public Module
{
public:
	
	FadeToBlack();

	// Destructor
	virtual ~FadeToBlack();

	// Called before render is available
	bool Awake(pugi::xml_node config);

	// Called after Awake
	bool Start();

	// Called every frame
	bool Update(float dt);

	// Called before quitting
	bool CleanUp();

	// CleanUp current scene and Init next Scene
	void NextScene();

	//Does the actual Fade
	void FadeBlack();

private:
	pugi::xml_node config;

	// To fade the window between scenes
	bool fadetoblack;
	Fade_Step currentStep = Fade_Step::NO;
	TypeScene typescene;

	// A frame count system to handle the fade time and ratio
	Uint32 frameCount = 0;
	Uint32 maxFadeFrames = 75;
	Uint32 maxFadeFramesBack = 0;
	
	SDL_Rect screenRect;
};

#endif // __ENTITYMANAGER_H__
