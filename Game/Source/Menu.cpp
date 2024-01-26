#include "Menu.h"
#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Render.h"
#include "Window.h"
#include "Level1.h"
#include "Settings.h"
#include "GUIManager.h"
#include "FadeToBlack.h"

#include "Defs.h"
#include "Log.h"

Menu::Menu() : Scene()
{
	name.Create("menu");
}

// Destructor
Menu::~Menu()
{}

// Called before render is available
bool Menu::Awake(pugi::xml_node config)
{
	LOG("Loading Intro");
	bool ret = true;

	this->config = config;

	isExiting = false;
	active = false;
	
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

	SDL_Rect buttonPos = { windowW / 2 + 200, windowH/2 - 250, 370, 115 };
	SDL_Rect section{ 0, 0, 360,115 };

	newGameButton = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 1, "New Game", buttonPos, section, this);
	menuGUI.Add(newGameButton);
	
	section.y = 145;
	section.w = 370;
	section.h = 115;
	buttonPos.y += 130;

	continueButton = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 2, "Continue", buttonPos, section, this);
	menuGUI.Add(continueButton);

	section.x = 400;
	section.y = 0;
	section.w = 370;
	section.h = 115;
	buttonPos.y += 130;

	settingsButton = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 3, "Settings", buttonPos, section, this);
	menuGUI.Add(settingsButton);
	
	section.x = 400;
	section.y = 145;
	section.w = 370;
	section.h = 115;
	buttonPos.y += 130;

	exitButton = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 4, "Exit", buttonPos, section, this);
	menuGUI.Add(exitButton);

	app->settings->Start();

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
	app->render->DrawTexture(fondoMenu, -260, -100, NULL);

	if (app->input->GetKey(SDL_SCANCODE_F4) == KEY_DOWN)
	{
		app->fade->FadeBlack((Module*)app->menu, (Module*)app->level_1, 10);
	}

	return true;
}

// Called each loop iteration
bool Menu::PostUpdate()
{
	bool ret = true;
	
	if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN || isExiting)
		ret = false;

	return ret;
}

// Called before quitting
bool Menu::CleanUp()
{
	LOG("Freeing Menu");

	ListItem<GuiControl*>* control = menuGUI.start;

	while (control != nullptr)
	{
		app->guiManager->DestroyGuiControl(control->data);
		control = control->next;
	}
	menuGUI.Clear();
	
	active = false;
	app->settings->CleanUp();

	return true;
}

bool Menu::OnGuiMouseClickEvent(GuiControl* control)
{
	// L15: DONE 5: Implement the OnGuiMouseClickEvent method
	LOG("Press Gui Control: %d", control->id);

	switch (control->id)
	{
	case 1:
		app->fade->FadeBlack((Module*)app->menu, (Module*)app->level_1, 10);
		break;
	case 2:
	case 3:
		app->settings->called = true;
		break;
	case 4:
		isExiting = true;
		break;
	default:
		break;
	}

	return true;
}
