#include "Player.h"
#include "App.h"
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

bool Player::Awake() {

	//Initialize Player parameters
	position = iPoint(config.attribute("x").as_int(), config.attribute("y").as_int());
	initPosition = position;
	speed = config.attribute("speed").as_int();

	//Initialize Walk Animation
	walkAnimation.speed = config.child("player").child("walkAnimation").attribute("speed").as_int();
	for (pugi::xml_node animationNode = config.child("player").child("walkAnimation").child("animation"); animationNode; animationNode = animationNode.next_sibling("animation"))
	{
		walkAnimation.PushBack({ animationNode.attribute("x").as_int(), animationNode.attribute("y").as_int(), animationNode.attribute("w").as_int(), animationNode.attribute("h").as_int() });
	}

	//Initialize Die Animation
	dieAnimation.speed = config.child("player").child("dieAnimation").attribute("speed").as_int();
	dieAnimation.loop = config.child("player").child("dieAnimation").attribute("loop").as_bool();
	for (pugi::xml_node animationNode = config.child("player").child("dieAnimation").child("animation"); animationNode; animationNode = animationNode.next_sibling("animation"))
	{
		dieAnimation.PushBack({ animationNode.attribute("x").as_int(), animationNode.attribute("y").as_int(), animationNode.attribute("w").as_int(), animationNode.attribute("h").as_int() });
	}

	currentAnimation = &walkAnimation;

	return true;
}

bool Player::Start() {

	texture = app->tex->Load(config.attribute("walktexturePath").as_string());

	app->tex->GetSize(texture, texW, texH);
	currentAnimation = &walkAnimation;
	pbody = app->physics->CreateCircle(position.x, position.y, currentAnimation->GetCurrentFrame().w-5, bodyType::DYNAMIC);

	//This makes the Physics module to call the OnCollision method
	pbody->listener = this;
	pbody->ctype = ColliderType::PLAYER;

	//initialize audio effect
	pickCoinFxId = app->audio->LoadFx(config.attribute("coinfxpath").as_string());

	death = false;
	flip = false;

	return true;
}

bool Player::Update(float dt)
{
	b2Vec2 velocity = b2Vec2(0, -GRAVITY_Y);

	cout << position.x << " " << position.y << endl;

	if (app->input->GetKey(SDL_SCANCODE_F1)  == KEY_DOWN || app->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN)
		respawn();

	if (app->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN && !god_mode)
	{
		god_mode = !god_mode;
		pbody->body->SetGravityScale(0.1);
	}
	else if (app->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN && god_mode)
	{
		god_mode = !god_mode;
		pbody->body->SetGravityScale(1);
	}

	if (death) 
	{
		if (dieAnimation.HasFinished())
		{
			dieAnimation.Reset();
			currentAnimation = &walkAnimation;
			respawn();
			death = false;
		}
	}

	if (!death) {

		b2ContactEdge* contact = pbody->body->GetContactList();
		if (contact != nullptr) {
			b2Vec2 contactPonts = contact->contact->GetManifold()->localNormal;
			if (contactPonts.y == -1) {
				jumpAnimation.Reset();
			}
		}
		if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT && god_mode)
		{
			velocity.x = -0.2 * dt;
			flip = true;
		}
		else if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
		{
			velocity.x = -0.4 * dt;
			flip = true;
		}

		if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT && god_mode)
		{
			velocity.x = 0.2 * dt;
			flip = false;
		}
		else if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
		{
			velocity.x = 0.4 * dt;
			flip = false;
		}

		if (app->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT && god_mode)
			velocity.y = -0.4 * dt;

		if (app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT && god_mode)
			velocity.y = 0.4 * dt;

		if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) 
		{
			if (contact != nullptr) {
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
		}

		if (remainingJumpSteps > 0) {
			float force = pbody->body->GetMass() * 10 / (1 / 30.0);
			force /= 3.5f;
			force -= jumpForceReduce;
			jumpForceReduce = maxJumpSteps - remainingJumpSteps;
			pbody->body->ApplyForceToCenter(b2Vec2(0, -(force * dt)), true);
			remainingJumpSteps--;
			if (jumpForceReduce > force) {
				remainingJumpSteps = 0;
				jumpForceReduce = 0;
			}
			if (contact != nullptr) {
				b2Vec2 contactPonts = contact->contact->GetManifold()->localNormal;
				if (contactPonts.y == 1) {
					force = 0;
					remainingJumpSteps = 0;
					jumpForceReduce = 0;
				}
			}
		}

		if (remainingJumpSteps <= 0) 
		{
			jumpAnimation.Reset();
		}
	}

	pbody->body->SetLinearVelocity(velocity);
	b2Transform pbodyPos = pbody->body->GetTransform();
	position.x = METERS_TO_PIXELS(pbodyPos.p.x) - texH / 2;
	position.y = METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2;

	if (!flip) 
		app->render->DrawTexture(texture, position.x + 12, position.y + 10, &currentAnimation->GetCurrentFrame());
	else 
		app->render->DrawTexturePR(texture, position.x + 12, position.y + 10, &currentAnimation->GetCurrentFrame());

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

	case ColliderType::ITEM:
		LOG("Collision ITEM");
		app->audio->PlayFx(pickCoinFxId);
		app->entityManager->DestroyEntity(physB->listener);
	
		break;

	case ColliderType::DEADLY:
		LOG("Collision DEADLY");
		if (!god_mode)
		{
			death = true;
			currentAnimation = &dieAnimation;
			texture = app->tex->Load(config.attribute("dietexturePath").as_string());
		}
		break;

	case ColliderType::UNKNOWN:
		LOG("Collision UNKNOWN");
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
	
	texture = app->tex->Load(config.attribute("walktexturePath").as_string());
}