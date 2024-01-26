#include "Settings.h"
#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Render.h"
#include "Window.h"
#include "Level1.h"
#include "GUIManager.h"
#include "FadeToBlack.h"

#include "Defs.h"
#include "Log.h"

Settings::Settings() : Scene()
{
	name.Create("settings");
}

// Destructor
Settings::~Settings()
{}

// Called before render is available
bool Settings::Awake(pugi::xml_node config)
{
	LOG("Loading Intro");
	bool ret = true;

	this->config = config;

	called = false;
	active = false;
	
	return ret;
}

// Called before the first frame
bool Settings::Start()
{
	//app->audio->PlayMusic("Assets/Audio/Music/music_spy.ogg");

	settingsMenu = app->tex->Load(config.attribute("texturePath").as_string());

	//Get the size of the window
	app->win->GetWindowSize(windowW, windowH);

	app->render->camera.x = 0;
	app->render->camera.y = 0;

	called = false;

	return true;
}

// Called each loop iteration
bool Settings::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool Settings::Update(float dt)
{
	if (called)
	{
		if (app->fade->typescene == TypeScene::MENU)
			app->render->DrawTexture(settingsMenu, windowW/2 -500, windowH/2 -250, NULL);
		else if (app->fade->typescene == TypeScene::LEVEL_1 || app->fade->typescene == TypeScene::LEVEL_2)
			app->render->DrawTexture(settingsMenu, windowW/2 +100, windowH/2 -250, NULL);
	}

	return true;
}

// Called each loop iteration
bool Settings::PostUpdate()
{
	bool ret = true;

	return ret;
}

// Called before quitting
bool Settings::CleanUp()
{
	LOG("Freeing In Game Menu");

	ListItem<GuiControl*>* control = settingGUI.start;

	while (control != nullptr)
	{
		app->guiManager->DestroyGuiControl(control->data);
		control = control->next;
	}
	settingGUI.Clear();
	
	active = false;

	return true;
}

bool Settings::OnGuiMouseClickEvent(GuiControl* control)
{
	// L15: DONE 5: Implement the OnGuiMouseClickEvent method
	LOG("Press Gui Control: %d", control->id);

	switch (control->id)
	{
	default:
		break;
	}

	return true;
}
