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

	// L07 DONE 6: Define OnCollision function for the player. 
	void OnCollision(PhysBody* physA, PhysBody* physB);

	void respawn();

public:
	float speed = 0.2f;
	SDL_Texture* texture = NULL;
	pugi::xml_node config;
	uint texW, texH;
	bool god_mode = false;
	bool flip;
	bool death;
	iPoint initPosition;

	//Audio fx
	int pickCoinFxId;

	// L07 DONE 5: Add physics to the player - declare a Physics body
	PhysBody* pbody;

	// jumping
	int remainingJumpSteps = 0;
	int jumpForceReduce = 0;
	int maxJumpSteps = 20;

	Animation* currentAnimation = nullptr;
	Animation walkAnimation;
	Animation jumpAnimation;
	Animation dieAnimation;
};

#endif // __PLAYER_H__