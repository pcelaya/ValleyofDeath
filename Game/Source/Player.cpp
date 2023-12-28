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
	idleAnimation.loop = config.child("idleAnimation").attribute("loop").as_bool();


	//Initialize Walk Animation
	for (pugi::xml_node animationNode = config.child("walkAnimation").child("animation"); animationNode; animationNode = animationNode.next_sibling("animation"))
	{
		walkAnimation.PushBack({ animationNode.attribute("x").as_int(), animationNode.attribute("y").as_int(), animationNode.attribute("w").as_int(), animationNode.attribute("h").as_int() });
	}
	walkAnimation.speed = config.child("walkAnimation").attribute("speed").as_float();
	walkAnimation.loop = config.child("walkAnimation").attribute("loop").as_bool();


	//Initialize Die Animation
	for (pugi::xml_node animationNode = config.child("dieAnimation").child("animation"); animationNode; animationNode = animationNode.next_sibling("animation"))
	{
		dieAnimation.PushBack({ animationNode.attribute("x").as_int(), animationNode.attribute("y").as_int(), animationNode.attribute("w").as_int(), animationNode.attribute("h").as_int() });
	}
	dieAnimation.speed = config.child("dieAnimation").attribute("speed").as_float();
	dieAnimation.loop = config.child("dieAnimation").attribute("loop").as_bool();


	// Initialize Jump Animation
	for (pugi::xml_node animationNode = config.child("jumpAnimation").child("animation"); animationNode; animationNode = animationNode.next_sibling("animation"))
	{
		jumpAnimation.PushBack({ animationNode.attribute("x").as_int(), animationNode.attribute("y").as_int(), animationNode.attribute("w").as_int(), animationNode.attribute("h").as_int() });
	}
	jumpAnimation.speed = config.child("jumpAnimation").attribute("speed").as_float();
	jumpAnimation.loop = config.child("jumpAnimation").attribute("loop").as_bool();


	// Initialize Attack Animation
	for (pugi::xml_node animnNode = config.child("attackAnimation").child("animation"); animnNode; animnNode = animnNode.next_sibling("animation"))
	{
		attackAnimation.PushBack({ animnNode.attribute("x").as_int(), animnNode.attribute("y").as_int(), animnNode.attribute("w").as_int(), animnNode.attribute("h").as_int() });
	}
	attackAnimation.speed = config.child("attackAnimation").attribute("speed").as_float();
	attackAnimation.loop = config.child("attackAnimation").attribute("loop").as_bool();

	return true;
}

bool Player::Start() {

	//initialize animation type
	texture = app->tex->Load(config.attribute("texturePath").as_string());
	state = IDLE;

	app->tex->GetSize(texture, texW, texH);
	currentAnimation = &idleAnimation;
	pbody = app->physics->CreateCircle(position.x, position.y, currentAnimation->GetCurrentFrame().w - 5, bodyType::DYNAMIC);

	//This makes the Physics module to call the OnCollision method
	pbody->listener = this;
	pbody->ctype = ColliderType::PLAYER;

	//initialize audio effect
	pickCoinFxId = app->audio->LoadFx(config.attribute("coinfxpath").as_string());

	dead = false;
	flip = false;
	god_mode = false;
	attack = false;

	return true;
}

bool Player::Update(float dt)
{
	b2Vec2 velocity = b2Vec2(0, -GRAVITY_Y);
	
	if (app->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN && !god_mode)
	{
		god_mode = !god_mode;
	}
	else if (app->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN && god_mode)
	{
		god_mode = !god_mode;
	}

	switch (state)
	{
	case IDLE:
		currentAnimation = &idleAnimation;
		break;
	case WALIKING:
		currentAnimation = &walkAnimation;
		break;
	case DIE:
		currentAnimation = &dieAnimation;
		break;
	case JUMP:
		currentAnimation = &jumpAnimation;
		break;
	case ATTACK:
		currentAnimation = &attackAnimation;
		break;
	default:
		break;
	}

	if (app->input->GetKey(SDL_SCANCODE_F1)  == KEY_DOWN || app->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN)
		respawn();
	if (god_mode)
	{
		velocity = b2Vec2(0, 0);
		pbody->body->SetGravityScale(0);
	}

	if (dead)
	{
		state = DIE;
		if (dieAnimation.HasFinished())
		{
			dieAnimation.Reset();
			state = IDLE;
			respawn();
			dead = false;
		}
	}
	else
	{
		if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) 
		{
			velocity.x = -0.2 * dt;
			flip = true;

			if (state == IDLE)
				state = WALIKING;
		}

		if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) 
		{
			velocity.x = 0.2 * dt;
			flip = false;

			if (state == IDLE)
				state = WALIKING;
		}

		if (app->input->GetKey(SDL_SCANCODE_D) == KEY_IDLE && app->input->GetKey(SDL_SCANCODE_A) == KEY_IDLE) 
		{
			if (state == WALIKING)
				state = IDLE;
		}

		if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) {
			if (jumps > 0) {
				state = JUMP;
				jumps--;
				remainJumpSteps = maxJumpSteps;
				jumpForceReduce = 1;
			}
		}

		if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN && !attack)
		{
			attack = true;
		}

		if (attack)
		{
			state = ATTACK;
			if (attackAnimation.HasFinished())
			{
				attackAnimation.Reset();
				state = IDLE;
				attack = false;
			}
		}

		if (remainJumpSteps > 0) {
			force = pbody->body->GetMass() * 10 / (1 / 30.0);
			force /= 2.7f;
			force -= jumpForceReduce;
			jumpForceReduce = maxJumpSteps - remainJumpSteps;
			pbody->body->ApplyForceToCenter(b2Vec2(0, -(force * dt)), true);
			remainJumpSteps--;
			if (jumpForceReduce > force) 
			{
				remainJumpSteps = 0;
				jumpForceReduce = 0;
			}
		}

		if (remainJumpSteps <= 0) {
			if (state == JUMP) {
				jumpAnimation.Reset();
				state = IDLE;
			}
			jumps = 2;
		}
	}
	
	if (!god_mode) {
		pbody->body->SetLinearVelocity(velocity);
		b2Transform pbodyPos = pbody->body->GetTransform();


		position.x = METERS_TO_PIXELS(pbodyPos.p.x) - texW / 2;
		position.y = METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2;
	}
	else {
		if (app->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) {
			position.y += -0.5 * dt;
		}
		if (app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) {
			position.y += 0.5 * dt;
		}
		if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
			position.x += -0.5 * dt;
		}
		if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
			position.x += 0.5 * dt;
		}
	}

	if (!flip)
		app->render->DrawTexture(texture, position.x + (texW / 2) -12, position.y + (texH / 2) -15, &currentAnimation->GetCurrentFrame());
	else
		app->render->DrawTexturePR(texture, position.x + (texW / 2) -12, position.y + (texH / 2) -15, &currentAnimation->GetCurrentFrame());

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

		if (contactPonts.y == 1) 
		{
			force = 0;
			remainJumpSteps = 0;
			jumpForceReduce = 0;
		}

		break;

	case ColliderType::ITEM:
		LOG("Collision ITEM");

		app->entityManager->DestroyEntity(physB->listener);
		app->audio->PlayFx(pickCoinFxId);

		break;

	case ColliderType::DEADLY:
		LOG("Collision DEADLY");

		if (!god_mode && state != ATTACK)
			dead = true;

		break;

	default:
		break;
	}
}

void Player::respawn()
{
	b2Vec2 initPos = b2Vec2(PIXEL_TO_METERS(initPosition.x), PIXEL_TO_METERS(initPosition.y));
	pbody->body->SetTransform(initPos, 0);
	position = initPosition;

	if (app->render->camera.x < 0)
		app->render->camera.x = 0;

	if (app->render->camera.y < 0)
		app->render->camera.y = 0;
}

iPoint Player::getTilePosition()
{
	return app->map->WorldToMap(position.x + (texW / 2) + (currentAnimation->GetCurrentFrame().w / 2), position.y + (texH / 2) + (currentAnimation->GetCurrentFrame().h / 2));
}
