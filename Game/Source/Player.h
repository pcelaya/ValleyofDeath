#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "Entity.h"
#include "Point.h"
#include "Animation.h"
#include "Render.h"
#include "SDL/include/SDL.h"

struct SDL_Texture;

class Player : public Entity
{
public:

	Player();
	
	virtual ~Player();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();
 
	void OnCollision(PhysBody* physA, PhysBody* physB);

	void respawn();

public:
	float speed = 0.2f;
	SDL_Texture* texture = NULL;
	uint texW, texH;
	bool god_mode = false;
	
	iPoint initPosition;
	PhysBody* pbody;

	// jumping
	int remainingJumpSteps = 0;
	int jumpForceReduce = 0;
	int maxJumpSteps = 20;

	// ANIMATION
	Animation* currentAnimation = nullptr;
	Animation idleAnimation;
	Animation walkAnimation;
	Animation jumpAnimation;
	Animation dieAnimation;
	Animation attackAnimation;
};

#endif // __PLAYER_H__