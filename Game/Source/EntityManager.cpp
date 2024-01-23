#include "EntityManager.h"
#include "Player.h"
#include "Physics.h"
#include "Item.h"
#include "App.h"
#include "Textures.h"
#include "Level1.h"
#include "Ghost.h"
#include "Skeleton.h"

#include "Defs.h"
#include "Log.h"

EntityManager::EntityManager() : Module()
{
	name.Create("entitymanager");
}

// Destructor
EntityManager::~EntityManager()
{}

// Called before render is available
bool EntityManager::Awake(pugi::xml_node config)
{
	LOG("Loading Entity Manager");
	bool ret = true;

	//Iterates over the entities and calls the Awake
	ListItem<Entity*>* item;
	Entity* pEntity = NULL;

	for (item = entities.start; item != NULL && ret == true; item = item->next)
	{
		pEntity = item->data;

		if (pEntity->active == false) continue;
		ret = item->data->Awake();
	}

	active = false;

	return ret;
}

bool EntityManager::Start() {

	bool ret = true; 

	//Iterates over the entities and calls Start
	ListItem<Entity*>* item;
	Entity* pEntity = NULL;

	for (item = entities.start; item != NULL && ret == true; item = item->next)
	{
		pEntity = item->data;

		if (pEntity->active == false) continue;
		ret = item->data->Start();
	}

	return ret;
}

// Called before quitting
bool EntityManager::CleanUp()
{
	bool ret = true;
	ListItem<Entity*>* item;
	item = entities.end;

	while (item != NULL && ret == true)
	{
		ret = item->data->CleanUp();
		item = item->prev;
	}

	entities.Clear();

	return ret;
}

Entity* EntityManager::CreateEntity(EntityType type)
{
	Entity* entity = nullptr; 

	//L03: DONE 3a: Instantiate entity according to the type and add the new entity to the list of Entities
	switch (type)
	{
	case EntityType::PLAYER:
		entity = new Player();
		break;
	case EntityType::ITEM:
		entity = new Item();
		break;
	case EntityType::GHOST:
		entity = new Ghost();
		break;
	case EntityType::SKELETON:
		entity = new Skeleton();
		break;

	default:
		break;
	}

	entities.Add(entity);

	return entity;
}

void EntityManager::DestroyEntity(Entity* entity)
{
	ListItem<Entity*>* item;

	for (item = entities.start; item != NULL; item = item->next)
	{
		if (item->data == entity) entities.Del(item);
	}
}

void EntityManager::AddEntity(Entity* entity)
{
	if ( entity != nullptr) entities.Add(entity);
}

bool EntityManager::LoadState(pugi::xml_node load)
{
	bool ret = true;

	//Iterates over the entities and calls Start
	ListItem<Entity*>* item;
	pugi::xml_node EntityNode = load.child("Enemy");

	for (item = entities.start; item != NULL && ret == true; item = item->next)
	{
		if (item->data->type == EntityType::ENEMY) 
			item->data->LoadEntity(EntityNode);
		else 
			item->data->LoadEntity(load);
	}

	return ret;
}

bool EntityManager::SaveState(pugi::xml_node save)
{
	bool ret = true;

	//Iterates over the entities and calls Start
	ListItem<Entity*>* item;

	for (item = entities.start; item != NULL && ret == true; item = item->next)
	{
		item->data->SaveEntity(save);
	}

	return ret;
}

bool EntityManager::Update(float dt)
{
	bool ret = true;
	ListItem<Entity*>* item;
	Entity* pEntity = NULL;

	for (item = entities.start; item != NULL && ret == true; item = item->next)
	{
		pEntity = item->data;

		if (pEntity->active == false) {
			continue;
		}
		/*if (pEntity->toDelete) {
			app->physics->DestroyBody(item->data->pbody->body);
			pEntity->toDelete = false;
			DestroyEntity(pEntity);
		}*/

		ret = item->data->Update(dt);
	}

	return ret;
}