#include "Skeleton.h"
#include "Scene.h"
#include "Log.h"
#include "Map.h"


Skeleton::Skeleton() : Enemy() {}

bool Skeleton::Awake()
{
	Enemy::Awake();

	texturePath = config.previous_sibling("groundEnemy").attribute("texturePath").as_string();
	pugi::xml_node parentConfig = config.previous_sibling("groundEnemy");

	// Initilize Skeleton Idle Animation
	pugi::xml_node animNode = parentConfig.child("idleAnimation").first_child();
	idleAnimation.PushBack({ animNode.attribute("x").as_int(), animNode.attribute("y").as_int(), animNode.attribute("w").as_int(), animNode.attribute("h").as_int() });
	idleAnimation.speed = parentConfig.child("idleAnimation").first_child().attribute("speed").as_float();
	idleAnimation.loop = parentConfig.child("idleAnimation").first_child().attribute("loop").as_bool();

	// Initialize Skeleton Walk Animation
	for (pugi::xml_node animnNode = parentConfig.child("walkAnimation").child("animation"); animnNode; animnNode = animnNode.next_sibling("animation"))
	{
		walkAnimation.PushBack({ animnNode.attribute("x").as_int(), animnNode.attribute("y").as_int(), animnNode.attribute("w").as_int(), animnNode.attribute("h").as_int() });
	}
	walkAnimation.speed = parentConfig.child("walkAnimation").attribute("speed").as_float();
	walkAnimation.loop = parentConfig.child("walkAnimation").attribute("loop").as_bool();

	// Initialize Skeleton Die Animation
	for (pugi::xml_node animnNode = parentConfig.child("dieAnimation").child("animation"); animnNode; animnNode = animnNode.next_sibling("animation"))
	{
		dieAnimation.PushBack({ animnNode.attribute("x").as_int(), animnNode.attribute("y").as_int(), animnNode.attribute("w").as_int(), animnNode.attribute("h").as_int() });
	}
	dieAnimation.speed = parentConfig.child("dieAnimation").attribute("speed").as_float();
	dieAnimation.loop = parentConfig.child("dieAnimation").attribute("loop").as_bool();
	
	// Initialize Skeleton Attack Animation
	for (pugi::xml_node animnNode = parentConfig.child("attackAnimation").child("animation"); animnNode; animnNode = animnNode.next_sibling("animation"))
	{
		attackAnimation.PushBack({ animnNode.attribute("x").as_int(), animnNode.attribute("y").as_int(), animnNode.attribute("w").as_int(), animnNode.attribute("h").as_int() });
	}
	attackAnimation.speed = parentConfig.child("attackAnimation").attribute("speed").as_float();
	attackAnimation.loop = parentConfig.child("attackAnimation").attribute("loop").as_bool();


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
	pbody->ctype = ColliderType::ENEMY;

	enemyRange = config.previous_sibling("groundEnemy").attribute("eRange").as_int();
	followVelovity = config.previous_sibling("groundEnemy").attribute("followVel").as_float();
	patrolVelocity = config.previous_sibling("groundEnemy").attribute("patrolVel").as_float();

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

	if (!hit)
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
			state = AnimSates::IDLE;
			dead = true;
			dieAnimation.Reset();
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
			hit = true;
		}
		break;

	case ColliderType::DEADLY:
		hit = true;
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

bool Skeleton::LoadEntity(pugi::xml_node& load)
{
	pugi::xml_node EnemyNode = load;

	dead = EnemyNode.attribute("dead").as_bool();
	hit = EnemyNode.attribute("dead").as_bool();

	if (hit)
	{
		position.x = -100;
		position.y = 200;

		dead = true;
	}
	else
	{
		position.x = EnemyNode.attribute("x").as_int();
		position.y = EnemyNode.attribute("y").as_int();

		b2Vec2 pPosition = b2Vec2(PIXEL_TO_METERS(position.x), PIXEL_TO_METERS(position.y));
		pbody->body->SetTransform(pPosition, 0);

		currentAnimation = &walkAnimation;
		dieAnimation.Reset();
	}
	load = EnemyNode.next_sibling("Enemy");


	return true;
}

bool Skeleton::SaveEntity(pugi::xml_node& save)
{
	pugi::xml_node EnemyNode = save.append_child(name.GetString());

	
	if (hit)
	{
		EnemyNode.append_attribute("dead").set_value(hit);
		return true;
	}

	EnemyNode.append_attribute("x").set_value(position.x);
	EnemyNode.append_attribute("y").set_value(position.y);
	EnemyNode.append_attribute("dead").set_value(hit);

	return true;
}
