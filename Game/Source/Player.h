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

	// Load and Saves Player position
	bool LoadEntity(pugi::xml_node& load);
	bool SaveEntity(pugi::xml_node& save);

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
	int fx1, fx2, fx3, pickCoinFxId;
};

#endif // __PLAYER_H__