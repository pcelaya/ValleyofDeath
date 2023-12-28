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

	// obtein the current tile which player stays
	iPoint getTilePosition();

public:
	float speed;
	SDL_Texture* texture = NULL;
	uint texW, texH;
	iPoint initPosition;
	AnimSates state;
	float force;
	bool god_mode = false;
	bool flip;
	bool dead;
	bool attack;

	// jumping
	PhysBody* pbody;
	int maxJumpSteps = 30;
	int remainJumpSteps = 0;
	int jumpForceReduce = 0;
	int jumps = 2;

	// ANIMATION
	Animation* currentAnimation = nullptr;
	Animation idleAnimation;
	Animation walkAnimation;
	Animation dieAnimation;
	Animation jumpAnimation;
	Animation attackAnimation;

	//Audio fx
	int pickCoinFxId;
	int endLevel;
};

#endif // __PLAYER_H__