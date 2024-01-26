#ifndef __BOSS_H__
#define __BOSS_H__

#include "Enemy.h"

class Boss : public Enemy
{
public:
	Boss();
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

	// Load and Saves Skeleton atributtes
	bool LoadEntity(pugi::xml_node& load);
	bool SaveEntity(pugi::xml_node& save);

private:
	int fx;
};

#endif