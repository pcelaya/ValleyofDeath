#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Level1.h"
#include "Map.h"
#include "InGameMenu.h"
#include "Settings.h"
#include "Ghost.h"
#include "Skeleton.h"
#include "Item.h"
#include "FadeToBlack.h"

#include "Defs.h"
#include "Log.h"

Level1::Level1() : Scene()
{
	name.Create("level_1");
}

// Destructor
Level1::~Level1()
{}

// Called before render is available
bool Level1::Awake(pugi::xml_node config)
{
	LOG("Loading Level 1");
	bool ret = true;

	app->render->config = config;

	//Get the map name from the config file and assigns the value in the module
	app->map->name = config.parent().child("map").child("level_1").attribute("name").as_string();
	app->map->path = config.parent().child("map").attribute("path").as_string();

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
bool Level1::Start()
{
	//app->audio->PlayMusic("Assets/Audio/Music/music_spy.ogg");

	//Get the size of the window
	app->win->GetWindowSize(windowW, windowH);

	app->render->camera.x = 0;
	app->render->camera.y = 0;

	limitCamera.right = app->render->camera.w - 40;
	limitCamera.left = 40;

	app->map->Start();
	app->entityManager->Start();
	app->ingame_menu->Start();
	app->settings->Start();

	return true;
}

// Called each loop iteration
bool Level1::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool Level1::Update(float dt)
{
	float camSpeed = 1;

	if (!debug && !player->god_mode)
	{
		LOG("leftCamera.left = %d", limitCamera.left);
		LOG("leftCamera.right = %d", limitCamera.right);
		LOG("Camera.x = %d", app->render->camera.x);
		LOG("player.x = %d\n", player->position.x + player->texW / 2);

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
		app->fade->FadeBlack((Module*) app->level_1, (Module*)app->level_2, 10);
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
bool Level1::PostUpdate()
{
	bool ret = true;

	if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		app->ingame_menu->called = true;

	return ret;
}

// Called before quitting
bool Level1::CleanUp()
{
	LOG("Freeing Level 1");

	app->map->active = false;
	app->map->CleanUp();
	app->entityManager->CleanUp();

	active = false;

	return true;
}
