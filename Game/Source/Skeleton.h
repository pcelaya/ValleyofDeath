#ifndef __SKELETON_H__
#define __SKELETON_H__

#include "Enemy.h"

class Skeleton : public Enemy
{
public:
	Skeleton();
	bool Awake();

	bool Start();

	bool Update(float dt);

	void moveToPlayer(float dt);

	void OnCollision(PhysBody* physA, PhysBody* physB);

	void moveToPoint(float dt);

	Animation idleAnimation;
	Animation walkAnimation;
	Animation dieAnimation;
	Animation attackAnimation;
};

#endif