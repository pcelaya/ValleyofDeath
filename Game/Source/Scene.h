#ifndef __SCENE_H__
#define __SCENE_H__

#include "Module.h"
#include "Player.h"
#include "Ghost.h"

struct SDL_Texture;

class Scene : public Module
{
public:

	Scene();

	// Destructor
	virtual ~Scene();

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
	float limitCamera;
	float limitWindow;

	// Debug mode
	bool debug;


	Player* player;
	Ghost* ghost;
	List<Entity>* enemy;
};

#endif // __SCENE_H__