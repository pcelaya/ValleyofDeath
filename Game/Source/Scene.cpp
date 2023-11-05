#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Scene.h"
#include "Map.h"
#include "Item.h"

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

	//L03: DONE 3b: Instantiate the player using the entity manager
	//L04 DONE 7: Get player paremeters
	player = (Player*) app->entityManager->CreateEntity(EntityType::PLAYER);
	//Assigns the XML node to a member in player
	player->config = config.child("player");

	//Get the map name from the config file and assigns the value in the module
	app->map->name = config.child("map").attribute("name").as_string();
	app->map->path = config.child("map").attribute("path").as_string();

	// iterate all items in the scene
	// Check https://pugixml.org/docs/quickstart.html#access
	for (pugi::xml_node itemNode = config.child("item"); itemNode; itemNode = itemNode.next_sibling("item"))
	{
		Item* item = (Item*)app->entityManager->CreateEntity(EntityType::ITEM);
		item->parameters = itemNode;
	}

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

	app->render->camera.y = ((player->position.y - player->texH / 2) - (app->scene->windowH / 2)) * -1 - 60;
	app->render->camera.x = ((player->position.x - player->texW / 2) - (app->scene->windowW / 2)) * -1 - 630;

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
	
	if (player->position.x > app->render->camera.w / 2)
	{
		app->render->camera.x = (((player->position.x - player->texW / 2) - (app->scene->windowW / 2)) * -1 - 630) * 2;
	}

	if(app->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		app->render->camera.y += (int)ceil(camSpeed * dt);

	if(app->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		app->render->camera.y -= (int)ceil(camSpeed * dt);

	if(app->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		app->render->camera.x += (int)ceil(camSpeed * dt);

	if(app->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		app->render->camera.x -= (int)ceil(camSpeed * dt);

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
