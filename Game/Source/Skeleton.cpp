#include "Skeleton.h"
#include "Scene.h"
#include "Log.h"

Skeleton::Skeleton() : Enemy() {}

bool Skeleton::Awake()
{
	Enemy::Awake();

	// Initialize Skeleton Walk Animation
	for (pugi::xml_node animnNode = config.child("walkAnimation").child("animation"); animnNode; animnNode = animnNode.next_sibling("animation"))
	{
		walkAnimation.PushBack({ animnNode.attribute("x").as_int(), animnNode.attribute("y").as_int(), animnNode.attribute("w").as_int(), animnNode.attribute("h").as_int() });
	}
	walkAnimation.speed = config.child("walkAnimation").attribute("speed").as_float();
	walkAnimation.loop = config.child("walkAnimation").attribute("loop").as_bool();

	return true;
}

bool Skeleton::Start()
{
	enemyRange = 10;
	initPosition = position;

	texture = app->tex->Load(texturePath);
	currentAnimation = &walkAnimation;
	pbody = app->physics->CreateCircle(position.x, position.y, currentAnimation->GetCurrentFrame().w - 5, bodyType::DYNAMIC);

	//This makes the Physics module to call the OnCollision method
	pbody->listener = this;
	pbody->ctype = ColliderType::DEADLY;

	followVelovity = 0.12;
	patrolVelocity = 0.1;

	Enemy::Start();

	return true;
}

bool Skeleton::Update(float dt)
{
	if (abs(app->scene->player->GetPlayerTileX() - getEnemyTileX()) > 50) {
		velocity.x = 0;
		velocity.y = 0;
		pbody->body->SetLinearVelocity(velocity);
		return true;
	}

	velocity = b2Vec2(0, -GRAVITY_Y);

	if (!dead)
	{
		if (canChase(enemyRange) && PTileX >= Patrol1.x && PTileX <= Patrol2.x && PTileY <= Patrol1.y) {
			realVelocity = followVelovity;
			dest = iPoint(PTileX, PTileY);
			moveToPlayer(dt);
			currentAnimation = &walkAnimation;
		}
		else {
			realVelocity = patrolVelocity;
			moveToPoint(dt);
		}
	}
	else
	{
		if (dieAnimation.HasFinished()) {
			dead = true;
		}
	}

	Enemy::Update(dt);

	return true;
}

void Skeleton::moveToPlayer(float dt)
{
	const DynArray<iPoint>* path = FindPath();

	if (path->Count() > 1) 
	{
		if (TileX > path->At(1)->x) 
			velocity.x = -realVelocity * dt;

		else
			velocity.x = realVelocity * dt;

	}
	else if (path->Count() == 1) 
	{
		if (app->scene->player->position.x < position.x) 
			velocity.x = -realVelocity * dt;

		else
			velocity.x = realVelocity * dt;
	}
}

void Skeleton::OnCollision(PhysBody* physA, PhysBody* physB)
{
	switch (physB->ctype)
	{
	case ColliderType::PLAYER:
		LOG("Collision PLAYER");
		dead = true;
		velocity.x = 0;
		currentAnimation = &dieAnimation;
		break;

	case ColliderType::DEADLY:
		dead = true;
		velocity.x = 0;
		currentAnimation = &dieAnimation;
		break;

	case ColliderType::PLATFORM:
		break;

	default:
		break;
	}
}

void Skeleton::moveToPoint(float dt)
{
	Enemy::Patrol();
	const DynArray<iPoint>* path = FindPath();
	//check if t has arrivied
	if (path->Count() > 1) {
		//check if it shall move to x
		if (TileX > path->At(1)->x) {
			velocity.x = -realVelocity * dt;
		}
		else {
			velocity.x = realVelocity * dt;
		}
	}
	else if (path->Count() == 1) {
		if (app->scene->player->position.x < position.x) {
			velocity.x = -realVelocity * dt;
		}
		else {
			velocity.x = realVelocity * dt;
		}
	}
}
