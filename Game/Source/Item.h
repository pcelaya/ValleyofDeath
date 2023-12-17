#ifndef __ITEM_H__
#define __ITEM_H__

#include "Entity.h"
#include "Point.h"
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

	bool isPicked = false;

private:

	SDL_Texture* texture;
	const char* texturePath;
	uint texW, texH;

	//L07 DONE 4: Add a physics to an item
	PhysBody* pbody;

	//Audio fx
	int pickCoinFxId;
};

#endif // __ITEM_H__