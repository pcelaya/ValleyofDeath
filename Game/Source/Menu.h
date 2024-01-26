#ifndef __MENU_H__
#define __MENU_H__

#include "Scene.h"
#include "GUIControl.h"
#include "GuiControlButton.h"
#include "List.h"

class Menu : public Scene
{
public:

	Menu();

	// Destructor
	virtual ~Menu();

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
	SDL_Texture* fondoMenu;

	GuiControlButton* newGameButton;
	GuiControlButton* continueButton;
	GuiControlButton* settingsButton;

	GuiControlButton* exitButton;
	bool isExiting;

	List<GuiControl*> menuGUI;
};

#endif // __INTRO_H__