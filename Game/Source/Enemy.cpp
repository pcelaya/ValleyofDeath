#include "Enemy.h"
#include "App.h"
#include "InGameMenu.h"
#include "Render.h"
#include "Map.h"
#include "Player.h"
#include "Level1.h"

Enemy::Enemy() : Entity(EntityType::ENEMY)
{
	name.Create("Enemy");
}

Enemy::~Enemy()
{
}

bool Enemy::Awake()
{
	position.x = config.attribute("x").as_int();
	position.y = config.attribute("y").as_int();
	initPosition = position;

	Patrolinit = { config.child("patrol").child("init").attribute("x").as_int(), config.child("patrol").child("init").attribute("y").as_int() };
	Patrolfinal = { config.child("patrol").child("final").attribute("x").as_int(), config.child("patrol").child("final").attribute("y").as_int() };

	return true;
}

bool Enemy::Start()
{
	
	mouseTileTex = app->tex->Load(config.parent().parent().child("renderer").attribute("pathTile").as_string());
	dead = false;
	hit = dead;
	patrol = true;
	return true;
}

bool Enemy::Update(float dt)
{
	if (!app->ingame_menu->called)
	{
		pbody->body->SetLinearVelocity(velocity);
		b2Transform pbodyPos = pbody->body->GetTransform();

		if (dead)
		{
			b2Vec2 diePos = b2Vec2(PIXEL_TO_METERS(-100), PIXEL_TO_METERS(200));
			position.x = METERS_TO_PIXELS(pbodyPos.p.x);
			position.y = METERS_TO_PIXELS(pbodyPos.p.y);
			pbody->body->SetTransform(diePos, 0);
		}
		else
		{
			position.x = METERS_TO_PIXELS(pbodyPos.p.x) - (currentAnimation->GetCurrentFrame().w / 2);
			position.y = METERS_TO_PIXELS(pbodyPos.p.y) - (currentAnimation->GetCurrentFrame().h / 2);
		}
	}

	if (velocity.x <= 0)
		app->render->DrawTexture(texture, position.x, position.y, &currentAnimation->GetCurrentFrame());
	else
		app->render->DrawTexturePR(texture, position.x, position.y, &currentAnimation->GetCurrentFrame());
	
	currentAnimation->Update();

	return true;
}

bool Enemy::CleanUp() {	return true; }

void Enemy::OnCollision(PhysBody* physA, PhysBody* physB) {}

const DynArray<iPoint>* Enemy::FindPath()
{
	iPoint origin = tilePos;

	app->map->pathfinding->CreatePath(origin, destiny, patrol);

	const DynArray<iPoint>* path = app->map->pathfinding->GetLastPath();

	if (app->level_1->debug)
	{
		for (uint i = 0; i < path->Count(); ++i)
		{
			iPoint pos = app->map->MapToWorld(path->At(i)->x, path->At(i)->y);
			app->render->DrawTexture(mouseTileTex, pos.x, pos.y);
		}
	}

	return path;
}

bool Enemy::canChase(int dist)
{
	bool canChase = false;

	tilePos = getTilePosition();

	ptilePos = app->level_1->player->getTilePosition();

	if ((abs(ptilePos.x - tilePos.x) + abs(ptilePos.y - tilePos.y)) < dist)
	{
		canChase = true;
	}

	if (canChase && app->level_1->player->god_mode == false)
	{
		canChase = true;
	}
	else 
	{
		canChase = false;
	}

	return canChase;
}

void Enemy::moveToPlayer(float dt){}

void Enemy::Patrol()
{
	if (patrol) 
	{
		destiny.x = Patrolinit.x;
		destiny.y = Patrolinit.y;
		if (tilePos.x == destiny.x && tilePos.y == destiny.y)
		{
			patrol = false;
		}
	}
	else 
	{
		destiny.x = Patrolfinal.x;
		destiny.y = Patrolfinal.y;
		if (tilePos.x == destiny.x && tilePos.y == destiny.y)
		{
			patrol = true;
		}
	}
}


iPoint Enemy::getTilePosition()
{
	return app->map->WorldToMap(position.x + (currentAnimation->GetCurrentFrame().w / 2), position.y + (currentAnimation->GetCurrentFrame().h / 2));
}