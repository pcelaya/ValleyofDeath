#include "Intro.h"
#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Render.h"
#include "Window.h"
#include "Menu.h"
#include "FadeToBlack.h"
#include "Audio.h"

#include "Defs.h"
#include "Log.h"

Intro::Intro() : Scene()
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
	app->audio->PlayMusic("Assets/Audio/Music/menu_theme.ogg");

	gameLogo = app->tex->Load(config.attribute("studioPath").as_string());
	introBackground = app->tex->Load(config.attribute("backintroPath").as_string());
	titlepageBackground = app->tex->Load(config.attribute("backtitlePath").as_string());

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
	app->render->DrawTexture(introBackground, 0, 0, NULL);
	app->render->DrawTexture(gameLogo, 0, 0, NULL);

	//app->render->DrawTextTitle(config.parent().first_child().first_child().child_value(), 0, 0, 1000, 250);
	//app->render->DrawTexture(titlepageBackground, 0, 0, NULL);

	if (app->input->GetKey(SDL_SCANCODE_F4) == KEY_DOWN)
	{
		app->fade->FadeBlack((Module*) app->intro, (Module*) app->menu, 10);
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
