#ifndef __SKELETON_H__
#define __SKELETON_H__

#include "Entity.h"
#include "Point.h"
#include "Animation.h"
#include "Render.h"
#include "SDL/include/SDL.h"

struct SDL_Texture;

class Skeleton : public Entity
{
public:

	Skeleton() : Entity(EntityType::SKELETON) {}

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	void OnCollision(PhysBody* physA, PhysBody* physB);

	//void respawn();

public:
	float speed = 0.2f;

	// ANIMATION
	Animation* currentAnimation = nullptr;
	Animation walkAnimation;
	Animation dieAnimation;
	Animation attackAnimation;
};

#endif // __SKELETON_H__


