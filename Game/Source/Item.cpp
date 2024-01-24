#include "Item.h"
#include "App.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Level1.h"
#include "Log.h"
#include "Point.h"
#include "Physics.h"
#include "Map.h"

Item::Item() : Entity(EntityType::ITEM)
{
	name.Create("item");
}

Item::~Item() {}

bool Item::Awake() {

	position.x = config.attribute("x").as_int();
	position.y = config.attribute("y").as_int();

	return true;
}

bool Item::Start() {

	//initilize textures
	texture = app->tex->Load(config.attribute("texturePath").as_string());
	
	// L07 DONE 4: Add a physics to an item - initialize the physics body
	app->tex->GetSize(texture, texW, texH);
	pbody = app->physics->CreateCircle(position.x + texH / 2, position.y + texH / 2, texH / 3, bodyType::DYNAMIC);

	// L07 DONE 7: Assign collider type
	pbody->listener = this;
	pbody->ctype = ColliderType::ITEM;
	pbody->body->SetGravityScale(0);
	isPicked = false;

	return true;
}

bool Item::Update(float dt)
{
	if (abs(app->level_1->player->getTilePosition().x - getTilePosition().x) > 100) {
		velocity.x = 0;
		velocity.y = 0;
		pbody->body->SetLinearVelocity(velocity);
		return true;
	}

	velocity = b2Vec2(0, 0);

	if (isPicked)
	{
		b2Vec2 diePos = b2Vec2(PIXEL_TO_METERS(-100), PIXEL_TO_METERS(200));
		b2Transform itemPos = pbody->body->GetTransform();
		position.x = METERS_TO_PIXELS(itemPos.p.x);
		position.y = METERS_TO_PIXELS(itemPos.p.x);
		pbody->body->SetTransform(diePos, 0);
	}

	pbody->body->SetLinearVelocity(velocity);
	b2Transform pbodyPos = pbody->body->GetTransform();

	position.x = METERS_TO_PIXELS(pbodyPos.p.x) - texH / 2;
	position.y = METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2;

	app->render->DrawTexture(texture, position.x, position.y, NULL);

	return true;
}

bool Item::CleanUp()
{
	return true;
}

void Item::OnCollision(PhysBody* physA, PhysBody* physB)
{
	switch (physB->ctype)
	{
	case ColliderType::PLATFORM:
		LOG("Collision PLATFORM");
		break;

	case ColliderType::PLAYER:
		LOG("Collision PLAYER");
		isPicked = true;
		break;

	default:
		break;
	}
}

iPoint Item::getTilePosition()
{
	return app->map->WorldToMap(position.x, position.y);
}