#ifndef __GHOST_H__
#define __GHOST_H__

#include "Entity.h"
#include "Point.h"
#include "Animation.h"
#include "Render.h"
#include "SDL/include/SDL.h"

struct SDL_Texture;

class Ghost : public Entity
{
public:

	Ghost() : Entity(EntityType::GHOST) {}

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	void OnCollision(PhysBody* physA, PhysBody* physB);


	//void respawn() override;

public:
	float speed;
	SDL_Texture* texture = NULL;
	uint texW, texH;
	SDL_Texture* mouseTileTex = nullptr;

	iPoint initPosition;
	int enemyRange;

	uint lives;

	// moving the enemy
	/*b2Vec2 velocity = b2Vec2(0, 0);
	float realVelocity;
	float followVelovity;
	float idleVelocity;*/

	// ANIMATION
	Animation* currentAnimation = nullptr;
	Animation walkAnimation;
	Animation dieAnimation;
	Animation attackAnimation;
};

#endif // __GHOST_H__