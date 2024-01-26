#ifndef __SETTINGS_H__
#define __SETTINGS_H__

#include "Scene.h"
#include "GUIControl.h"
#include "GuiControlButton.h"
#include "List.h"

class Settings : public Scene
{
public:

	Settings();

	// Destructor
	virtual ~Settings();

	// Called before render is available
	bool Awake(pugi::xml_node config);

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	bool OnGuiMouseClickEvent(GuiControl* control);

public:
	pugi::xml_node config;

	SDL_Texture* settingsMenu;
	bool called;

	List<GuiControl*> settingGUI;
};

#endif // __SETTINGS_H__