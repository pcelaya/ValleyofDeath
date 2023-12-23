#include "Ghost.h"
#include "Scene.h"


Ghost::Ghost() : Enemy() {}


bool Ghost::Awake()
{
	Enemy::Awake();

	// Initialize Ghost Flying animation
	for (pugi::xml_node animnNode = config.child("flyAnimation").child("animation"); animnNode; animnNode = animnNode.next_sibling("animation"))
	{
		flyAnimation.PushBack({ animnNode.attribute("x").as_int(), animnNode.attribute("y").as_int(), animnNode.attribute("w").as_int(), animnNode.attribute("h").as_int() });
	}
	flyAnimation.speed = config.child("flyAnimation").attribute("speed").as_int();


	// Initialize Ghost Attack animation
	for (pugi::xml_node animnNode = config.child("attackAnimation").child("animation"); animnNode; animnNode = animnNode.next_sibling("animation"))
	{
		attackAnimation.PushBack({ animnNode.attribute("x").as_int(), animnNode.attribute("y").as_int(), animnNode.attribute("w").as_int(), animnNode.attribute("h").as_int() });
	}
	attackAnimation.speed = config.child("attackAnimation").attribute("speed").as_int();


	//Initialize Ghost Die animation
	for (pugi::xml_node animationNode = config.child("dieAnimation").child("animation"); animationNode; animationNode = animationNode.next_sibling("animation"))
	{
		dieAnimation.PushBack({ animationNode.attribute("x").as_int(), animationNode.attribute("y").as_int(), animationNode.attribute("w").as_int(), animationNode.attribute("h").as_int() });
	}
	dieAnimation.speed = config.child("dieAnimation").attribute("speed").as_float();

	return true;
}

bool Ghost::Start()
{
	texture = app->tex->Load(texturePath);
	currentAnimation = &flyAnimation;
	pbody = app->physics->CreateCircle(position.x, position.y, currentAnimation->GetCurrentFrame().w /2, bodyType::DYNAMIC);

	//This makes the Physics module to call the OnCollision method
	pbody->listener = this;
	pbody->ctype = ColliderType::DEADLY;

	enemyRange = 20;
	pbody->body->SetGravityScale(0);
	followVelovity = 0.15;
	patrolVelocity = 0.1;

	Enemy::Start();

	return true;
}

bool Ghost::Update(float dt)
{
	if (abs(app->scene->player->GetPlayerTileX() - getEnemyTileX()) > 50) {
		velocity.x = 0;
		velocity.y = 0;
		pbody->body->SetLinearVelocity(velocity);
		return true;
	}

	velocity = b2Vec2(0, 0);

	if (!dead)
	{	
		if (canChase(enemyRange)) {
			realVelocity = followVelovity;
			dest = iPoint(PTileX, PTileY);
			moveToPlayer(dt);
			currentAnimation = &flyAnimation;
		}
		else {
			realVelocity = patrolVelocity;
			Enemy::Patrol();
			moveToPlayer(dt);
			currentAnimation = &flyAnimation;
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

void Ghost::moveToPlayer(float dt)
{
	const DynArray<iPoint>* path = FindPath();

	if (path->Count() > 1) 
	{
		if (TileX != path->At(1)->x) 
		{
			if (TileX > path->At(1)->x) 
				velocity.x = -realVelocity * dt;

			else 
				velocity.x = realVelocity * dt;

			if (path->Count() > 2) 
			{
				if (path->At(2)->y != TileY) 
				{
					if (TileY > path->At(2)->y)
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
			if (TileY > path->At(1)->y) 
				velocity.y = -realVelocity * dt;
			
			else 
				velocity.y = realVelocity * dt;
			

			if (path->Count() > 2) 
			{
				if (path->At(2)->x != TileX) 
				{
					if (TileX > path->At(2)->x) 
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
		dead = true;
		currentAnimation = &dieAnimation;
		break;
	default:
		break;
	}
}