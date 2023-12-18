#include "Ghost.h"
#include "App.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Map.h"
#include "Log.h"
#include "Point.h"
#include "Physics.h"



bool Ghost::Awake()
{
	position = iPoint(config.attribute("x").as_int(), config.attribute("y").as_int());
	initPosition = position;
	speed = config.attribute("speed").as_int();

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
	texture = app->tex->Load(config.attribute("flytexturePath").as_string());

	app->tex->GetSize(texture, texW, texH);
	currentAnimation = &flyAnimation;
	pbody = app->physics->CreateCircle(position.x - texW/2, position.y - texH / 2, currentAnimation->GetCurrentFrame().w /2, bodyType::DYNAMIC);

	//This makes the Physics module to call the OnCollision method
	pbody->listener = this;
	pbody->ctype = ColliderType::DEADLY;

	enemyRange = 20;

	//idleVelocity = speed;
	//followVelovity = speed * 1.5;
	active = true;
	flip = false;

	mouseTileTex = app->tex->Load(config.parent().parent().child("renderer").child("pathTile").attribute("texturepath").as_string());

	return true;
}

bool Ghost::Update(float dt)
{
	b2Vec2 velocity = b2Vec2(0, 0);
	tilePos = app->map->WorldToMap(position.x + texW/2, position.y + texH/2);

	pbody->body->SetLinearVelocity(velocity);
	b2Transform pbodyPos = pbody->body->GetTransform();
	position.x = METERS_TO_PIXELS(pbodyPos.p.x) - texH / 2;
	position.y = METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2;

	if (!active)
	{
		if (dieAnimation.HasFinished())
		{
			dieAnimation.Reset();
			currentAnimation = &flyAnimation;
		}
	}
	else
	{	
		iPoint origin = tilePos;
		iPoint destiny = app->scene->player->tilePos;

		if (destiny.DistanceTo(origin) < enemyRange && app->scene->player->active)
		{
			//realVelocity = followVelovity;

			app->map->pathfinding->CreatePath(origin, destiny);
			const DynArray<iPoint>* movePath = app->map->pathfinding->GetLastPath();
			/*if (movePath->Count() > 1)
			{
				if (tilePos.x != movePath->At(1)->x)
				{
					if (tilePos.x > movePath->At(1)->x)
						velocity.x = -realVelocity * dt;
					else 
						velocity.x = realVelocity * dt;

					if (movePath->Count() > 2)
					{
						if (movePath->At(2)->y != tilePos.y)
						{
							if (tilePos.y > movePath->At(2)->y)
								velocity.y = -realVelocity / 1.3 * dt;
							else
								velocity.y = realVelocity / 1.3 * dt;

							velocity.x /= 1.3;
						}
						else
							velocity.y = 0;
					}
				}
			}
			else
			{
				if (tilePos.y != movePath->At(1)->y) 
				{
				
				}
			}*/
		}

		if (app->scene->debug)
		{
			if (app->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_REPEAT)
			{
				//Get the latest calculated path and draw
				const DynArray<iPoint>* path = app->map->pathfinding->GetLastPath();
				for (uint i = 0; i < path->Count(); ++i)
				{
					iPoint pos = app->map->MapToWorld(path->At(i)->x, path->At(i)->y);
					app->render->DrawTexture(mouseTileTex, pos.x, pos.y);
				}
			}
		}

		if (!flip)
			app->render->DrawTexture(texture, position.x + 12, position.y + 9, &currentAnimation->GetCurrentFrame());
		else
			app->render->DrawTexturePR(texture, position.x + 12, position.y + 9, &currentAnimation->GetCurrentFrame());
	}

	currentAnimation->Update();

	return true;
}

bool Ghost::CleanUp()
{
	return true;
}

void Ghost::OnCollision(PhysBody* physA, PhysBody* physB)
{
	b2ContactEdge* contact = pbody->body->GetContactList();
	b2Vec2 contactPonts = contact->contact->GetManifold()->localNormal;

	switch (physB->ctype)
	{
	case ColliderType::PLATFORM:
		LOG("Collision PLATFORM");
		break;

	case ColliderType::PLAYER:
		LOG("Collision PLAYER");
		break;

	default:
		break;
	}
}