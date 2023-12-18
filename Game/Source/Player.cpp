#include "Player.h"
#include "App.h"
#include "Map.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Point.h"
#include "Physics.h"


Player::Player() : Entity(EntityType::PLAYER)
{
	name.Create("Player");
}

Player::~Player() {}

bool Player::Awake() 
{
	//Initialize Player parameters
	position = iPoint(config.attribute("x").as_int(), config.attribute("y").as_int());
	initPosition = position;
	speed = config.attribute("speed").as_float();

	//Initialize Idle Animation
	for (pugi::xml_node animationNode = config.child("idleAnimation").child("animation"); animationNode; animationNode = animationNode.next_sibling("animation"))
	{
		idleAnimation.PushBack({ animationNode.attribute("x").as_int(), animationNode.attribute("y").as_int(), animationNode.attribute("w").as_int(), animationNode.attribute("h").as_int() });
	}
	idleAnimation.speed = config.child("idleAnimation").attribute("speed").as_float();

	// Initialize Jump Animation
	for (pugi::xml_node animationNode = config.child("jumpAnimation").child("animation"); animationNode; animationNode = animationNode.next_sibling("animation"))
	{
		jumpAnimation.PushBack({ animationNode.attribute("x").as_int(), animationNode.attribute("y").as_int(), animationNode.attribute("w").as_int(), animationNode.attribute("h").as_int() });
	}
	jumpAnimation.speed = config.child("jumpAnimation").attribute("speed").as_float();

	//Initialize Walk Animation
	for (pugi::xml_node animationNode = config.child("walkAnimation").child("animation"); animationNode; animationNode = animationNode.next_sibling("animation"))
	{
		walkAnimation.PushBack({ animationNode.attribute("x").as_int(), animationNode.attribute("y").as_int(), animationNode.attribute("w").as_int(), animationNode.attribute("h").as_int() });
	}
	walkAnimation.speed = config.child("walkAnimation").attribute("speed").as_float();

	// Initialize Attack Animation
	for (pugi::xml_node animnNode = config.child("attackAnimation").child("animation"); animnNode; animnNode = animnNode.next_sibling("animation"))
	{
		attackAnimation.PushBack({ animnNode.attribute("x").as_int(), animnNode.attribute("y").as_int(), animnNode.attribute("w").as_int(), animnNode.attribute("h").as_int() });
	}
	attackAnimation.speed = config.child("attackAnimation").attribute("speed").as_int();
	attackAnimation.loop = config.child("attackAnimation").attribute("loop").as_bool();

	//Initialize Die Animation
	for (pugi::xml_node animationNode = config.child("dieAnimation").child("animation"); animationNode; animationNode = animationNode.next_sibling("animation"))
	{
		dieAnimation.PushBack({ animationNode.attribute("x").as_int(), animationNode.attribute("y").as_int(), animationNode.attribute("w").as_int(), animationNode.attribute("h").as_int() });
	}
	dieAnimation.speed = config.child("dieAnimation").attribute("speed").as_float();
	dieAnimation.loop = config.child("dieAnimation").attribute("loop").as_bool();

	return true;
}

bool Player::Start() {

	//initialize animation type
	texture = app->tex->Load(config.attribute("idletexturePath").as_string());

	//initialize audio effect
	pickCoinFxId = app->audio->LoadFx(config.attribute("coinfxpath").as_string());

	app->tex->GetSize(texture, texW, texH);
	currentAnimation = &idleAnimation;
	pbody = app->physics->CreateCircle(position.x, position.y, currentAnimation->GetCurrentFrame().w - 5, bodyType::DYNAMIC);

	//This makes the Physics module to call the OnCollision method
	pbody->listener = this;
	pbody->ctype = ColliderType::PLAYER;

	dead = false;
	flip = false;
	god_mode = false;
	onAir = false;
	attack = false;

	return true;
}

bool Player::Update(float dt)
{
	b2Vec2 velocity = b2Vec2(0, -GRAVITY_Y);
	tilePos = app->map->WorldToMap(position.x +texW/4, position.y +texH / 4);

	if (app->input->GetKey(SDL_SCANCODE_F1)  == KEY_DOWN || app->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN)
		respawn();

	if (app->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN && !god_mode)
	{
		god_mode = !god_mode;
	}
	else if (app->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN && god_mode)
	{
		god_mode = !god_mode;
	}

	if (god_mode)
	{
		velocity = b2Vec2(0, 0);
		pbody->body->SetGravityScale(0);
	}

	if (dead) 
	{
		texture = app->tex->Load(config.attribute("dietexturePath").as_string());
		currentAnimation = &dieAnimation;
	}
	else
	{
		b2ContactEdge* contact = pbody->body->GetContactList();
		if (contact != nullptr) {
			b2Vec2 contactPonts = contact->contact->GetManifold()->localNormal;
			if (contactPonts.y == -1) {
				jumpAnimation.Reset();
			}
		}

		if (god_mode)
		{
			if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
			{
				velocity.x = -0.4 * dt;
				texture = app->tex->Load(config.attribute("walktexturePath").as_string());
				currentAnimation = &walkAnimation;
				flip = true;
			}
			else if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
			{
				velocity.x = 0.4 * dt;
				texture = app->tex->Load(config.attribute("walktexturePath").as_string());
				currentAnimation = &walkAnimation;
				flip = false;
			}
			else
			{
				texture = app->tex->Load(config.attribute("idletexturePath").as_string());
				currentAnimation = &idleAnimation;
				walkAnimation.Reset();
			}

			if (app->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
				velocity.y = -0.4 * dt;

			if (app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
				velocity.y = 0.4 * dt;
		}
		else
		{
			if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
			{
				velocity.x = -0.2 * dt;
				texture = app->tex->Load(config.attribute("walktexturePath").as_string());
				currentAnimation = &walkAnimation;
				flip = true;
			}
			else if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
			{
				velocity.x = 0.2 * dt;
				texture = app->tex->Load(config.attribute("walktexturePath").as_string());
				currentAnimation = &walkAnimation;
				flip = false;
			}
			else if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
			{
				attack = true;
				texture = app->tex->Load(config.attribute("attacktexturePath").as_string());
				currentAnimation = &attackAnimation;
			}
		}

		if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && !onAir)
		{
			if (contact != nullptr)
			{
				texture = app->tex->Load(config.attribute("jumptexturePath").as_string());
				currentAnimation = &jumpAnimation;
				b2Vec2 contactPonts = contact->contact->GetManifold()->localNormal;
				if (contactPonts.y == 0 && (contactPonts.x == 1 || contactPonts.x == -1))
				{
					remainingJumpSteps = maxJumpSteps;
					jumpForceReduce = 0;
				}
				else
				{
					remainingJumpSteps = maxJumpSteps;
					jumpForceReduce = 0;
				}
			}
			else {
				remainingJumpSteps = maxJumpSteps;
				jumpForceReduce = 0;
			}

			onAir = true;
		}
		if (remainingJumpSteps > 0)
		{
			float force = pbody->body->GetMass() * 10 / (1 / 30.0);
			force /= 2.8f;

			force -= jumpForceReduce;
			jumpForceReduce = maxJumpSteps - remainingJumpSteps;

			pbody->body->ApplyForceToCenter(b2Vec2(0, -(force * dt)), true);
			remainingJumpSteps--;

			if (jumpForceReduce > force)
			{
				remainingJumpSteps = 0;
				jumpForceReduce = 0;
			}
			if (contact != nullptr)
			{
				b2Vec2 contactPonts = contact->contact->GetManifold()->localNormal;
				if (contactPonts.y == 1) {
					force = 0;
					remainingJumpSteps = 0;
					jumpForceReduce = 0;
				}
			}
		}
		else if (remainingJumpSteps <= 0)
		{
			jumpAnimation.Reset();
			onAir = false;
		}
	}
	
	if (dieAnimation.HasFinished())
	{
		respawn();
	}

	if (attack)
	{
		if (attackAnimation.HasFinished())
		{
			attack = false;
			attackAnimation.Reset();
		}
	}

	pbody->body->SetLinearVelocity(velocity);
	b2Transform pbodyPos = pbody->body->GetTransform();
	position.x = METERS_TO_PIXELS(pbodyPos.p.x) - texH / 2;
	position.y = METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2;


	if (!flip)
		app->render->DrawTexture(texture, position.x+12, position.y+5, &currentAnimation->GetCurrentFrame());
	else
		app->render->DrawTexturePR(texture, position.x+12, position.y+5, &currentAnimation->GetCurrentFrame());

	currentAnimation->Update();

	return true;
}

bool Player::CleanUp()
{
	return true;
}

void Player::OnCollision(PhysBody* physA, PhysBody* physB) 
{
	b2ContactEdge* contact = pbody->body->GetContactList();
	b2Vec2 contactPonts = contact->contact->GetManifold()->localNormal;

	switch (physB->ctype)
	{
	case ColliderType::PLATFORM:
		LOG("Collision PLATFORM");
		break;

	case ColliderType::DEADLY:
		LOG("Collision DEADLY");
		if (!god_mode)
			dead = true;
		break;
	default:
		break;
	}
}

void Player::respawn()
{
	dead = false;
	dieAnimation.Reset();

	b2Vec2 initPos = b2Vec2(PIXEL_TO_METERS(initPosition.x), PIXEL_TO_METERS(initPosition.y));
	pbody->body->SetTransform(initPos, 0);
	position = initPosition;

	if (app->render->camera.x < 0)
		app->render->camera.x = 0;
	if (app->render->camera.y < 0)
		app->render->camera.y = 0;
	
	texture = app->tex->Load(config.attribute("idletexturePath").as_string());
	currentAnimation = &idleAnimation;
}
