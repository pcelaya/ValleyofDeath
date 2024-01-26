#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Level2.h"
#include "Map.h"
#include "Ghost.h"
#include "Skeleton.h"
#include "Item.h"

#include "Defs.h"
#include "Log.h"

Level2::Level2() : Scene()
{
	name.Create("level_2");
}

// Destructor
Level2::~Level2()
{}

// Called before render is available
bool Level2::Awake(pugi::xml_node config)
{
	LOG("Loading Level 2");
	bool ret = true;

	player = (Player*)app->entityManager->CreateEntity(EntityType::PLAYER);
	//Assigns the XML node to a member in player
	player->config = config.child("player");

	// iterate all ghost in the scene
	for (pugi::xml_node enemyNode = config.child("ghost"); enemyNode; enemyNode = enemyNode.next_sibling("ghost"))
	{
		Ghost* ghost = (Ghost*)app->entityManager->CreateEntity(EntityType::GHOST);
		ghost->config = enemyNode;
	}

	// iterate all ghost in the scene
	for (pugi::xml_node enemyNode = config.child("skeleton"); enemyNode; enemyNode = enemyNode.next_sibling("skeleton"))
	{
		Skeleton* skele = (Skeleton*)app->entityManager->CreateEntity(EntityType::SKELETON);
		skele->config = enemyNode;
	}

	//Get the map name from the config file and assigns the value in the module
	app->map->name = config.parent().child("map").attribute("name").as_string();
	app->map->path = config.parent().child("map").attribute("path").as_string();

	//// iterate all items in the scene
	//// Check https://pugixml.org/docs/quickstart.html#access
	for (pugi::xml_node itemNode = config.child("item"); itemNode; itemNode = itemNode.next_sibling("item"))
	{
		Item* item = (Item*)app->entityManager->CreateEntity(EntityType::ITEM);
		item->config = itemNode;
	}

	debug = false;
	active = false;

	return ret;
}

// Called before the first frame
bool Level2::Start()
{
	//app->audio->PlayMusic("Assets/Audio/Music/music_spy.ogg");

	//Get the size of the window
	app->win->GetWindowSize(windowW, windowH);

	app->render->camera.x = 0;
	app->render->camera.y = 0;

	limitCamera.right = app->render->camera.w - 40;
	limitCamera.left = 40;

	return true;
}

// Called each loop iteration
bool Level2::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool Level2::Update(float dt)
{
	float camSpeed = 1;

	if (!debug && !player->god_mode)
	{
		if (player->position.x + player->texW / 2 > limitCamera.right)
		{
			app->render->camera.x -= windowW - 80;
			limitCamera.left = limitCamera.right;
			limitCamera.right = (app->render->camera.x * -1) + app->render->camera.w - 40;
		}
		/*else if (player->position.x + player->texW / 2 < limitCamera.left && limitCamera.left > windowW - 40)
		{
			app->render->camera.x += 2222;
		}*/
	}
	else
	{
		if (app->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
			app->render->camera.x += (int)ceil(camSpeed * dt);

		if (app->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
			app->render->camera.x -= (int)ceil(camSpeed * dt);
	}

	if (app->input->GetKey(SDL_SCANCODE_F4) == KEY_DOWN)
	{
		//fade
		// Scene* level_2 = (Scene*) level2
		//currentstep = TO_BLACK;
		// maxFadeFrames = 200;
	}
	
	// Request to Load or Save game information
	if (app->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
		app->SaveRequest();
	if (app->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN)
		app->LoadRequest();

	// Activate or deactivate debug mode
	if (app->input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN && !debug)
	{
		debug = !debug;
	}
	else if (app->input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN && debug)
	{
		debug = !debug;

		if (!player->god_mode)
		{
			if (app->render->camera.x < 0)
				app->render->camera.x = 0;

			if (app->render->camera.y < 0)
				app->render->camera.y = 0;
		}
	}
	

	return true;
}

// Called each loop iteration
bool Level2::PostUpdate()
{
	bool ret = true;

	if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	return ret;
}

// Called before quitting
bool Level2::CleanUp()
{
	LOG("Freeing Level 1");

	app->map->CleanUp();
	app->entityManager->CleanUp();

	active = false;

	return true;
}