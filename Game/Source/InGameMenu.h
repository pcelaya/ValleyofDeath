#ifndef __INGAMEMENU_H__
#define __INGAMEMENU_H__

#include "Scene.h"
#include "GUIControl.h"
#include "GuiControlButton.h"
#include "List.h"

class InGameMenu : public Scene
{
public:

	InGameMenu();

	// Destructor
	virtual ~InGameMenu();

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
	int menuPosX;

	SDL_Texture* bookMenu;
	bool called;
	bool resume;

	GuiControlButton* resumeButton;
	GuiControlButton* settingsButton;
	GuiControlButton* backtoTitleButton;

	GuiControlButton* exitButton;
	bool isExiting;

	List<GuiControl*> ingame_menuGUI;
};

#endif // __INTRO_H__