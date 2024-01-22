#include "Intro.h"
#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Render.h"
#include "Window.h"

#include "Defs.h"
#include "Log.h"

Intro::Intro() : Module()
{
	name.Create("intro");
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
	app->render->DrawTexture(gameLogo, -35, 0, NULL);

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

	return true;
}
