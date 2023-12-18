#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "Entity.h"
#include "Point.h"
#include "Animation.h"
#include "Render.h"
#include "SDL/include/SDL.h"

struct SDL_Texture;

enum class AnimationType
{
	IDLE,
	WALK,
	JUMP,
	ATTACK,
	DIE,
};

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

	void animationManager();
public:
	float speed;
	bool god_mode = false;
	bool moveing;

	//b2Vec2 velocity;

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
	AnimationType animtype;
	AnimationType currentype;

	//Audio fx
	int pickCoinFxId;
};

#endif // __PLAYER_H__