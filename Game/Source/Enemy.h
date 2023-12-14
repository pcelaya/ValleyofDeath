#ifndef __ENEMY_H__
#define __ENEMY_H__

#include "Entity.h"
#include "Point.h"
#include "Animation.h"
#include "Render.h"
#include "SDL/include/SDL.h"

struct SDL_Texture;

class Enemy : public Entity
{
public:

	Enemy();

	virtual ~Enemy();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	void OnCollision(PhysBody* physA, PhysBody* physB);

	void respawn();

public:
	float speed = 0.2f;
	SDL_Texture* texture = NULL;
	pugi::xml_node config;
	uint texW, texH;

	bool flip;
	bool death;
	iPoint initPosition;
	PhysBody* pbody;

	// ANIMATION
	Animation* currentAnimation = nullptr;
	Animation walkAnimation;
	Animation dieAnimation;
	Animation attackAnimation;
};

#endif // __ENEMY_H__