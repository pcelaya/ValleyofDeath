#ifndef __ENEMY_H__
#define __ENEMY_H__

#include "Entity.h"
#include "App.h"
#include "Animation.h"
#include "Textures.h"
#include "Physics.h"
#include "List.h"
#include "DynArray.h"

class Enemy : public Entity
{
public:
	Enemy();

	virtual ~Enemy();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	// L07 DONE 6: Define OnCollision function for the player. 
	virtual void OnCollision(PhysBody* physA, PhysBody* physB);

	const DynArray<iPoint>* FindPath();

	bool canChase(int dist);

	virtual void moveToPlayer(float dt);

	void Patrol();

	// obtain tile position of enemy
	iPoint getTilePosition();

protected:
	const char* texturePath;
	uint texW, texH;
	SDL_Texture* texture;
	Animation* currentAnimation = nullptr;
	AnimSates state;
	PhysBody* pbody;
	SDL_Texture* mouseTileTex;

	iPoint initPosition;
	iPoint ptilePos;

	b2Vec2 velocity;
	float realVelocity;
	float followVelovity;
	float patrolVelocity;
	
	iPoint Patrolinit;
	iPoint Patrolfinal;
	bool patrol;

	int enemyRange;
	iPoint destiny;

	bool dead;
	bool hit;
	int lives;

};

#endif // __ENEMY_H__