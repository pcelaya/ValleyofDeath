#ifndef __ITEM_H__
#define __ITEM_H__

#include "Entity.h"
#include "Point.h"
#include "Physics.h"
#include "SDL/include/SDL.h"

struct SDL_Texture;

class Item : public Entity
{
public:

	Item();
	virtual ~Item();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	void OnCollision(PhysBody* physA, PhysBody* physB);

	// obtain tile position of enemy
	iPoint getTilePosition();

	bool isPicked = false;

private:
	b2Vec2 velocity;
	uint texW, texH;
	SDL_Texture* texture;
};

#endif // __ITEM_H__