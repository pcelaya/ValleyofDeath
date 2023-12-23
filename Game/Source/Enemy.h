#ifndef __ENEMY_H__
#define __ENEMY_H__

#include "Entity.h"
#include "App.h"
#include "Animation.h"
#include "Textures.h"
#include "Physics.h"
#include "List.h"
#include "DynArray.h"

enum class EnemyType
{
	GHOST,
	SKELETON,
	UNKNOWN
};

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

	int getEnemyTileX();

	int getEnemyTileY();

	const DynArray<iPoint>* FindPath();

	bool canChase(int dist);

	virtual void moveToPlayer(float dt);

	void Patrol();

	bool dark;

protected:
	const char* texturePath;
	SDL_Texture* texture;
	int lives;
	Animation* currentAnimation = nullptr;
	PhysBody* pbody;
	iPoint initPosition;
	SDL_Texture* mouseTileTex;
	float realVelocity;
	float followVelovity;
	float patrolVelocity;
	bool dead;
	bool hit;
	bool flip;

	int enemyRange;
	iPoint dest;

	int TileX;
	int TileY;
	int PTileX;
	int PTileY;

	b2Vec2 velocity;

	iPoint Patrol1;
	iPoint Patrol2;
	bool patrol;

};

#endif // __ENEMY_H__