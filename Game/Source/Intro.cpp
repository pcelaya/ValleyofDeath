#include "Intro.h"
#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Render.h"
#include "Window.h"

//delete post adding Menu
#include "Level1.h"

#include "Defs.h"
#include "Log.h"

Intro::Intro() : Scene()
{
	name.Create("intro");
	activeScene = true;
}

// Destructor
Intro::~Intro()
{}

// Called before render is available
bool Intro::Awake(pugi::xml_node config)
{
	LOG("Loading Intro");
	bool ret = true;

	//app->render->config = config;
	this->config = config;

	return ret;
}

// Called before the first frame
bool Intro::Start()
{
	//app->audio->PlayMusic("Assets/Audio/Music/music_spy.ogg");

	gameLogo = app->tex->Load(config.attribute("texturePath").as_string());

	//Get the size of the window
	app->win->GetWindowSize(windowW, windowH);

	app->render->camera.x = 0;
	app->render->camera.y = 0;

	return true;
}

// Called each loop iteration
bool Intro::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool Intro::Update(float dt)
{
	if (!activeScene)
		return true;

	app->render->DrawTexture(gameLogo, 0, 0, NULL);
	if (app->input->GetKey(SDL_SCANCODE_F4) == KEY_DOWN)
	{
		app->level_1->activeScene = true;
		activeScene = false;
	}

	return true;
}

// Called each loop iteration
bool Intro::PostUpdate()
{
	bool ret = true;
	
	if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	return ret;
}

// Called before quitting
bool Intro::CleanUp()
{
	LOG("Freeing Level 1");

	active = false;

	return true;
}
