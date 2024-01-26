#ifndef __LEVEL2_H__
#define __LEVEL2_H__

#include "Scene.h"
#include "Player.h"


class Level2 : public Scene
{
public:

	Level2();

	// Destructor
	virtual ~Level2();

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
	// Debug mode
	bool debug;

	Player* player;
};

#endif // __LEVEL1_H__