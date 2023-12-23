#include "Enemy.h"
#include "Log.h"
#include "Render.h"
#include "Map.h"
#include "Player.h"
#include "Scene.h"

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
	texturePath = config.attribute("texturePath").as_string();
	Patrol1 = { config.attribute("dest1X").as_int(), config.attribute("dest1Y").as_int() };
	Patrol2 = { config.attribute("dest2X").as_int(), config.attribute("dest2Y").as_int() };
	return true;
}

bool Enemy::Start()
{
	mouseTileTex = app->tex->Load(config.parent().parent().child("renderer").attribute("pathTile").as_string());
	dead = false;
	hit = false;
	patrol = true;
	return true;
}

bool Enemy::Update(float dt)
{
	pbody->body->SetLinearVelocity(velocity);
	b2Transform pbodyPos = pbody->body->GetTransform();

	if (dead) {
		b2Vec2 diePos = b2Vec2(PIXEL_TO_METERS(0), PIXEL_TO_METERS(0));
		pbody->body->SetTransform(diePos, 0);
	}
	else {
		position.x = METERS_TO_PIXELS(pbodyPos.p.x) - (currentAnimation->GetCurrentFrame().w / 2);
		position.y = METERS_TO_PIXELS(pbodyPos.p.y) - (currentAnimation->GetCurrentFrame().h / 2);
	}

	if (!dead) 
	{
		if (!flip)
			app->render->DrawTexture(texture, position.x, position.y, &currentAnimation->GetCurrentFrame());
		else
			app->render->DrawTexturePR(texture, position.x, position.y, &currentAnimation->GetCurrentFrame());
	}

	currentAnimation->Update();

	if (dead)
	{
		toDelete = true;
		// mirar de borrar texture i memory leaks
		//app->entityManager->DestroyEntity(this);
	}

	return true;
}

bool Enemy::CleanUp() {	return true; }

void Enemy::OnCollision(PhysBody* physA, PhysBody* physB) {}

int Enemy::getEnemyTileX()
{
	return (position.x + (currentAnimation->GetCurrentFrame().w / 2)) / app->map->GetTileWidth();
}

int Enemy::getEnemyTileY()
{
	return (position.y + (currentAnimation->GetCurrentFrame().h / 2)) / app->map->GetTileHeight();
}

const DynArray<iPoint>* Enemy::FindPath()
{
	iPoint origin = iPoint(TileX, TileY);

	app->map->pathfinding->CreatePath(origin, dest);

	const DynArray<iPoint>* path = app->map->pathfinding->GetLastPath();
	if (app->scene->debug && app->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_REPEAT)
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

	TileX = getEnemyTileX();
	TileY = getEnemyTileY();

	PTileX = app->scene->player->GetPlayerTileX();
	PTileY = app->scene->player->GetPlayerTileY();

	if ((abs(TileX - PTileX) + abs(TileY - PTileY)) < dist) 
	{
		canChase = true;
	}

	if (canChase && !app->scene->player->god_mode) 
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
		dest.x = Patrol1.x;
		dest.y = Patrol1.y;
		if (getEnemyTileX() == dest.x) 
		{
			patrol = false;
		}
	}
	else 
	{
		dest.x = Patrol2.x;
		dest.y = Patrol2.y;
		if (getEnemyTileX() == dest.x) 
		{
			patrol = true;
		}
	}
}
