#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Scene.h"
#include "Map.h"
#include "Skeleton.h"

#include "Defs.h"
#include "Log.h"

Scene::Scene() : Module()
{
	name.Create("scene");
}

// Destructor
Scene::~Scene()
{}

// Called before render is available
bool Scene::Awake(pugi::xml_node config)
{
	LOG("Loading Scene");
	bool ret = true;

	app->render->config = config;

	player = (Player*) app->entityManager->CreateEntity(EntityType::PLAYER);
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
	//for (pugi::xml_node itemNode = config.child("item"); itemNode; itemNode = itemNode.next_sibling("item"))
	//{
	//	Item* item = (Item*)app->entityManager->CreateEntity(EntityType::ITEM);
	//	item->config = itemNode;
	//}

	debug = false;

	return ret;
}

// Called before the first frame
bool Scene::Start()
{
	//app->audio->PlayMusic("Assets/Audio/Music/music_spy.ogg");

	//Get the size of the window
	app->win->GetWindowSize(windowW, windowH);

	textPosX = (float)windowW / 2 - (float)texW / 2;
	textPosY = (float)windowH / 2 - (float)texH / 2;

	app->render->camera.x = 0;
	app->render->camera.y = 0;

	return true;
}

// Called each loop iteration
bool Scene::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool Scene::Update(float dt)
{
	float camSpeed = 1; 
	
	limitCamera = player->position.x - windowW + 32;
	if (limitCamera > 0 && limitCamera < (app->map->GetMapWidth() - windowW) && limitCamera < app->map->GetMapWidth() && !player->god_mode)
		app->render->camera.x = (player->position.x - windowW / 2) * -1; 

	if(app->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		app->render->camera.x += (int)ceil(camSpeed * dt);

	if(app->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		app->render->camera.x -= (int)ceil(camSpeed * dt);

	//if (app->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
	//	app->render->camera.y += (int)ceil(camSpeed * dt);
	//if (app->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
	//	app->render->camera.y -= (int)ceil(camSpeed * dt);

	if (app->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN) 
		app->SaveRequest();
	if (app->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN) 
		app->LoadRequest();

	return true;
}

// Called each loop iteration
bool Scene::PostUpdate()
{
	bool ret = true;

	if(app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	return ret;
}

// Called before quitting
bool Scene::CleanUp()
{
	LOG("Freeing scene");

	return true;
}
