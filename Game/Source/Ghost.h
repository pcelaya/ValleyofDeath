#ifndef __GHOST_H__
#define __GHOST_H__

#include "Enemy.h"

struct SDL_Texture;

class Ghost : public Enemy
{
public:

	Ghost();

	bool Awake();

	bool Start();

	bool Update(float dt);

	void moveToPlayer(float dt);

	void OnCollision(PhysBody* physA, PhysBody* physB);

	// ANIMATION
	Animation flyAnimation;
	Animation dieAnimation;
	Animation attackAnimation;

	// Load and Saves Ghosts atributtes
	bool LoadEntity(pugi::xml_node& load);
	bool SaveEntity(pugi::xml_node& save);

private:
	int fx;
};

#endif // __GHOST_H__