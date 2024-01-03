#include "Ghost.h"
#include "Scene.h"
#include "Log.h"
#include "Map.h"


Ghost::Ghost() : Enemy() {}


bool Ghost::Awake()
{
	Enemy::Awake();

	texturePath = config.previous_sibling("flyingEnemy").attribute("texturePath").as_string();
	pugi::xml_node parentConfig = config.previous_sibling("flyingEnemy");

	// Initialize Ghost Flying animation
	for (pugi::xml_node animnNode = parentConfig.child("flyAnimation").child("animation"); animnNode; animnNode = animnNode.next_sibling("animation"))
	{
		flyAnimation.PushBack({ animnNode.attribute("x").as_int(), animnNode.attribute("y").as_int(), animnNode.attribute("w").as_int(), animnNode.attribute("h").as_int() });
	}
	flyAnimation.speed = parentConfig.child("flyAnimation").attribute("speed").as_int();

	//Initialize Ghost Die animation
	for (pugi::xml_node animationNode = parentConfig.child("dieAnimation").child("animation"); animationNode; animationNode = animationNode.next_sibling("animation"))
	{
		dieAnimation.PushBack({ animationNode.attribute("x").as_int(), animationNode.attribute("y").as_int(), animationNode.attribute("w").as_int(), animationNode.attribute("h").as_int() });
	}
	dieAnimation.speed = parentConfig.child("dieAnimation").attribute("speed").as_float();

	return true;
}

bool Ghost::Start()
{
	texture = app->tex->Load(texturePath);
	state = AnimSates::WALIKING;

	app->tex->GetSize(texture, texW, texH);
	currentAnimation = &flyAnimation;
	pbody = app->physics->CreateCircle(position.x, position.y, currentAnimation->GetCurrentFrame().w /2, bodyType::DYNAMIC);

	//This makes the Physics module to call the OnCollision method
	pbody->listener = this;
	pbody->ctype = ColliderType::ENEMY;
	pbody->body->SetGravityScale(0);

	enemyRange = config.previous_sibling("flyingEnemy").attribute("eRange").as_int();
	followVelovity = config.previous_sibling("flyingEnemy").attribute("followVel").as_float();
	patrolVelocity = config.previous_sibling("flyingEnemy").attribute("patrolVel").as_float();

	Enemy::Start();

	return true;
}

bool Ghost::Update(float dt)
{
	if (abs(app->scene->player->getTilePosition().x - getTilePosition().x) > 100) {
		velocity.x = 0;
		velocity.y = 0;
		pbody->body->SetLinearVelocity(velocity);
		return true;
	}

	velocity = b2Vec2(0, 0);
	
	switch (state)
	{
	case AnimSates::WALIKING:
		currentAnimation = &flyAnimation;
		break;
	case AnimSates::DIE:
		currentAnimation = &dieAnimation;
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
		}
		else {
			realVelocity = patrolVelocity;
			Enemy::Patrol();
			moveToPlayer(dt);
		}
	}
	else
	{
		state = AnimSates::DIE;
		if (dieAnimation.HasFinished()) {
			dieAnimation.Reset();
			state = AnimSates::WALIKING;
			dead = false;
		}
	}

	Enemy::Update(dt);

	return true;
}

void Ghost::moveToPlayer(float dt)
{
	const DynArray<iPoint>* path = FindPath();

	if (path->Count() > 1) 
	{
		if (tilePos.x != path->At(1)->x) 
		{
			if (tilePos.x > path->At(1)->x)
				velocity.x = -realVelocity * dt;

			else 
				velocity.x = realVelocity * dt;

			if (path->Count() > 2) 
			{
				if (path->At(2)->y != tilePos.y)
				{
					if (tilePos.y > path->At(2)->y)
					{
						velocity.y = -realVelocity / 1.3 * dt;
					}
					else 
					{
						velocity.y = realVelocity / 1.3 * dt;
					}

					velocity.x /= 1.3;

				}
				else 
					velocity.y = 0;
				
			}
		}
		else {
			if (tilePos.y > path->At(1)->y)
				velocity.y = -realVelocity * dt;
			
			else 
				velocity.y = realVelocity * dt;
			

			if (path->Count() > 2) 
			{
				if (path->At(2)->x != tilePos.x)
				{
					if (tilePos.x > path->At(2)->x)
					{
						velocity.x = -realVelocity / 1.3 * dt;
					}
					else 
					{
						velocity.x = realVelocity / 1.3 * dt;
					}
					velocity.y /= 1.3;
				}
				else 
					velocity.x = 0;
				
			}
		}
	}
	else if (path->Count() == 1) 
	{
		if (app->scene->player->position.x < position.x) 
			velocity.x = -realVelocity * dt;
		
		else 
			velocity.x = realVelocity * dt;
		


		if (app->scene->player->position.y < position.y) 
			velocity.y = -realVelocity * dt;
		
		else 
			velocity.y = realVelocity * dt;
	}
}


void Ghost::OnCollision(PhysBody* physA, PhysBody* physB)
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

	case ColliderType::PLATFORM:
		break;

	case ColliderType::ENEMY:
		break;

	default:
		break;
	}
}

bool Ghost::LoadEntity(pugi::xml_node& load)
{
	pugi::xml_node EnemyNode = load;

	dead = EnemyNode.attribute("dead").as_bool();

	if (dead) 
	{
		position.x = 0;
		position.y = 0;

		b2Vec2 diePos = b2Vec2(PIXEL_TO_METERS(0), PIXEL_TO_METERS(0));
		pbody->body->SetTransform(diePos, 0);

		hit = true;
	}
	else 
	{
		position.x = EnemyNode.attribute("x").as_int();
		position.y = EnemyNode.attribute("y").as_int();

		b2Vec2 pPosition = b2Vec2(PIXEL_TO_METERS(position.x), PIXEL_TO_METERS(position.y));
		pbody->body->SetTransform(pPosition, 0);

		currentAnimation = &flyAnimation;
		hit = false;
		dieAnimation.Reset();
	}
	load = EnemyNode.next_sibling("Enemy");


	return true;
}

bool Ghost::SaveEntity(pugi::xml_node& save)
{
	pugi::xml_node EnemyNode = save.append_child(name.GetString());

	EnemyNode.append_attribute("x").set_value(position.x);
	EnemyNode.append_attribute("y").set_value(position.y);

	EnemyNode.append_attribute("dead").set_value(dead);

	return true;
}
