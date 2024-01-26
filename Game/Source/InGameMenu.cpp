#include "InGameMenu.h"
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

InGameMenu::InGameMenu() : Scene()
{
	name.Create("ingame_menu");
}

// Destructor
InGameMenu::~InGameMenu()
{}

// Called before render is available
bool InGameMenu::Awake(pugi::xml_node config)
{
	LOG("Loading Intro");
	bool ret = true;

	this->config = config;

	isExiting = false;
	called = false;
	resume = false;
	active = false;
	
	return ret;
}

// Called before the first frame
bool InGameMenu::Start()
{
	//app->audio->PlayMusic("Assets/Audio/Music/music_spy.ogg");

	bookMenu = app->tex->Load(config.attribute("texturePath").as_string());

	//Get the size of the window
	app->win->GetWindowSize(windowW, windowH);

	app->render->camera.x = 0;
	app->render->camera.y = 0;

	menuPosX = -510;

	SDL_Rect buttonPos = { menuPosX + 100, windowH / 2 - 250, 370, 115 };
	SDL_Rect section{ 0, 0, 360,115 };

	resumeButton = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 5, "Resume", buttonPos, section, this);
	ingame_menuGUI.Add(resumeButton);

	section.y = 145;
	section.w = 370;
	section.h = 115;
	buttonPos.y += 130;

	settingsButton = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 6, "Settings", buttonPos, section, this);
	ingame_menuGUI.Add(settingsButton);

	section.x = 400;
	section.y = 0;
	section.w = 370;
	section.h = 115;
	buttonPos.y += 130;

	backtoTitleButton = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 7, "Menu", buttonPos, section, this);
	ingame_menuGUI.Add(backtoTitleButton);

	section.x = 400;
	section.y = 145;
	section.w = 370;
	section.h = 115;
	buttonPos.y += 130;

	exitButton = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 8, "Exit", buttonPos, section, this);
	ingame_menuGUI.Add(exitButton);

	return true;
}

// Called each loop iteration
bool InGameMenu::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool InGameMenu::Update(float dt)
{
	if (called)
	{
		app->render->DrawTexture(bookMenu, menuPosX, 0, NULL);

		if (menuPosX < 0)
		{
			resumeButton->buttonX += 10;
			settingsButton->buttonX += 10;
			backtoTitleButton->buttonX += 10;
			exitButton->buttonX += 10;
			menuPosX += 10;
		}
	}

	if (resume)
	{
		if (menuPosX > -510)
		{
			resumeButton->buttonX -= 10;
			settingsButton->buttonX -= 10;
			backtoTitleButton->buttonX -= 10;
			exitButton->buttonX -= 10;
			menuPosX -= 10;
		}
		else
			called = false;
	}

	return true;
}

// Called each loop iteration
bool InGameMenu::PostUpdate()
{
	bool ret = true;
	
	if (isExiting)
		ret = false;

	return ret;
}

// Called before quitting
bool InGameMenu::CleanUp()
{
	LOG("Freeing In Game Menu");

	ListItem<GuiControl*>* control = ingame_menuGUI.start;

	while (control != nullptr)
	{
		app->guiManager->DestroyGuiControl(control->data);
		control = control->next;
	}
	ingame_menuGUI.Clear();
	
	active = false;

	return true;
}

bool InGameMenu::OnGuiMouseClickEvent(GuiControl* control)
{
	// L15: DONE 5: Implement the OnGuiMouseClickEvent method
	LOG("Press Gui Control: %d", control->id);

	switch (control->id)
	{
	case 5:
		resume = true;
		break;
	case 6:
		app->settings->called = true;
		break;
	case 7:
		app->fade->FadeBlack((Module*) app->level_1, (Module*) app->menu, 10, true);
	case 8:
		isExiting = true;
		break;
	default:
		break;
	}

	return true;
}
