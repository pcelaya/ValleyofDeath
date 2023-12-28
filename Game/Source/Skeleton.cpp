#include "Skeleton.h"
#include "Scene.h"
#include "Log.h"
#include "Map.h"


Skeleton::Skeleton() : Enemy() {}

bool Skeleton::Awake()
{
	Enemy::Awake();

	// Initilize Skeleton Idle Animation
	pugi::xml_node animNode = config.child("idleAnimation").first_child();
	idleAnimation.PushBack({ animNode.attribute("x").as_int(), animNode.attribute("y").as_int(), animNode.attribute("w").as_int(), animNode.attribute("h").as_int() });
	idleAnimation.speed = config.child("idleAnimation").first_child().attribute("speed").as_float();
	idleAnimation.loop = config.child("idleAnimation").first_child().attribute("loop").as_float();

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
	texture = app->tex->Load(texturePath);
	state = AnimSates::IDLE;

	app->tex->GetSize(texture, texW, texH);
	currentAnimation = &idleAnimation;
	pbody = app->physics->CreateCircle(position.x, position.y, currentAnimation->GetCurrentFrame().w -5, bodyType::DYNAMIC);

	//This makes the Physics module to call the OnCollision method
	pbody->listener = this;
	pbody->ctype = ColliderType::DEADLY;

	enemyRange = 20;
	followVelovity = 0.12;
	patrolVelocity = 0.1;

	Enemy::Start();

	return true;
}

bool Skeleton::Update(float dt)
{
	if (abs(app->scene->player->getTilePosition().x - getTilePosition().x) > 100) {
		velocity.x = 0;
		velocity.y = 0;
		pbody->body->SetLinearVelocity(velocity);
		return true;
	}

	velocity = b2Vec2(0, -GRAVITY_Y);
	
	switch (state)
	{
	case AnimSates::IDLE:
		currentAnimation = &idleAnimation;
		break;
	case AnimSates::WALIKING:
		currentAnimation = &walkAnimation;
		break;
	case AnimSates::DIE:
		currentAnimation = &dieAnimation;
		break;
	case AnimSates::ATTACK:
		currentAnimation = &attackAnimation;
		break;
	default:
		break;
	}

	if (!dead)
	{
		if (canChase(enemyRange)) {
			realVelocity = followVelovity;
			destiny = ptilePos;
			moveToPlayer(dt);

			if (state == AnimSates::IDLE)
				state = AnimSates::WALIKING;
		}
		else {
			realVelocity = patrolVelocity;
			moveToPoint(dt);

			if (state == AnimSates::IDLE)
				state = AnimSates::WALIKING;
		}
	}
	else
	{
		state = AnimSates::DIE;
		if (dieAnimation.HasFinished()) 
		{
			dieAnimation.Reset();
			state = AnimSates::IDLE;
			dead = false;
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
		if (tilePos.x > path->At(1)->x) 
			velocity.x = -realVelocity * dt;

		else
			velocity.x = realVelocity * dt;

	}
	else if (path->Count() == 1) 
	{
		state = AnimSates::ATTACK;
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
		if (app->scene->player->state == AnimSates::ATTACK)
		{
			dead = true;
		}
		break;

	case ColliderType::DEADLY:
		dead = true;
		break;

	case ColliderType::PLATFORM:
		break;

	case ColliderType::ENEMY:
		break;

	default:
		break;
	}
}

void Skeleton::moveToPoint(float dt)
{
	Enemy::Patrol();

	const DynArray<iPoint>* path = FindPath();

	if (path->Count() > 1)
	{
		if (tilePos.x > path->At(1)->x)
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
