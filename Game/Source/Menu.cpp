#include "Menu.h"
#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Render.h"
#include "Window.h"
#include "Level1.h"
#include "GUIManager.h"

#include "Defs.h"
#include "Log.h"

Menu::Menu() : Scene()
{
	name.Create("intro");
	activeScene = true;
}

// Destructor
Menu::~Menu()
{}

// Called before render is available
bool Menu::Awake(pugi::xml_node config)
{
	LOG("Loading Intro");
	bool ret = true;

	//app->render->config = config;
	this->config = config;

	return ret;
}

// Called before the first frame
bool Menu::Start()
{
	//app->audio->PlayMusic("Assets/Audio/Music/music_spy.ogg");

	fondoMenu = app->tex->Load(config.attribute("texturePath").as_string());

	//Get the size of the window
	app->win->GetWindowSize(windowW, windowH);

	app->render->camera.x = 0;
	app->render->camera.y = 0;

	SDL_Rect buttonPos = { windowW / 2 - 60, windowH / 2 - 80, 120, 20 };
	playButton = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 1, "Play", buttonPos, this);
	return true;
}

// Called each loop iteration
bool Menu::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool Menu::Update(float dt)
{
	if (!activeScene)
		return true;

	app->render->DrawTexture(fondoMenu, 0, 0, NULL);
	if (app->input->GetKey(SDL_SCANCODE_F4) == KEY_DOWN)
	{
		app->level_1->activeScene = true;
		activeScene = false;
	}

	return true;
}

// Called each loop iteration
bool Menu::PostUpdate()
{
	bool ret = true;
	
	if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	return ret;
}

// Called before quitting
bool Menu::CleanUp()
{
	LOG("Freeing Level 1");

	active = false;

	return true;
}

bool Menu::OnGuiMouseClickEvent(GuiControl* control)
{
	// L15: DONE 5: Implement the OnGuiMouseClickEvent method
	LOG("Press Gui Control: %d", control->id);

	return true;
}
