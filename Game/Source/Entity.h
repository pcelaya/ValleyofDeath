#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "Point.h"
#include "SString.h"
#include "Input.h"
#include "Render.h"

enum class EntityType
{
	PLAYER,
	ITEM,
	ENEMY,
	GHOST,
	SKELETON,
	UNKNOWN
};

enum class AnimSates 
{
	IDLE,
	WALIKING,
	DIE,
	JUMP,
	ATTACK
};


class PhysBody;

class Entity
{
public:

	Entity(EntityType type) : type(type), active(true) {}

	virtual bool Awake()
	{
		return true;
	}

	virtual bool Start()
	{
		return true;
	}

	virtual bool Update(float dt)
	{
		return true;
	}

	virtual bool CleanUp()
	{
		return true;
	}

	virtual bool LoadEntity(pugi::xml_node&)
	{
		return true;
	}

	virtual bool SaveEntity(pugi::xml_node&)
	{
		return true;
	}

	void Entity::Enable()
	{
		if (!active)
		{
			active = true;
			Start();
		}
	}

	void Entity::Disable()
	{
		if (active)
		{
			active = false;
			CleanUp();
		}
	}

	virtual void OnCollision(PhysBody* physA, PhysBody* physB) {};

public:
	SString name;
	EntityType type;
	bool active = true;
	pugi::xml_node config;

	PhysBody* pbody;
	iPoint tilePos;

	bool renderable = true;
	iPoint position;
};

#endif // __ENTITY_H__