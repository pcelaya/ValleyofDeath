#include "GuiControlButton.h"
#include "Render.h"
#include "App.h"
#include "Audio.h"
#include "Textures.h"

GuiControlButton::GuiControlButton(uint32 id, SDL_Rect bounds, SDL_Rect section, const char* text) : GuiControl(GuiControlType::BUTTON, id)
{
	this->bounds = bounds;

	this->tex = section;

	this->buttonX = bounds.x;
	this->buttonY = bounds.y;

	this->text = text;

	canClick = true;
	drawBasic = false;

	gui_Buttons = app->tex->Load("Assets/Textures/gui_Buttons.png");
}

GuiControlButton::~GuiControlButton()
{

}

bool GuiControlButton::Update(float dt)
{
	if (state != GuiControlState::DISABLED)
	{
		// L15: DONE 3: Update the state of the GUiButton according to the mouse position
		app->input->GetMousePosition(mouseX, mouseY);

		//If the position of the mouse if inside the bounds of the button 
		if (mouseX > bounds.x && mouseX < bounds.x + bounds.w && mouseY > bounds.y && mouseY < bounds.y + bounds.h) {
		
			state = GuiControlState::FOCUSED;

			if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT) 
				state = GuiControlState::PRESSED;
			
			if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP) 
				NotifyObserver();
		}
		else 
			state = GuiControlState::NORMAL;
	}

	tex2 = tex;
	tex2.x += 800;

	switch (state)
	{
	case GuiControlState::DISABLED:
		app->render->DrawTexture(gui_Buttons, buttonX, buttonY, &tex2);
		break;
	case GuiControlState::NORMAL:
		app->render->DrawTexture(gui_Buttons, buttonX, buttonY, &tex);
		app->render->DrawTextGUI(text.GetString(), buttonX +45, buttonY+10, bounds.w - 100, bounds.h - 30);
		break;
	case GuiControlState::FOCUSED:
		app->render->DrawTexture(gui_Buttons, buttonX, buttonY, &tex2);
		app->render->DrawTextGUI(text.GetString(), buttonX +45, buttonY + 10, bounds.w - 100, bounds.h - 30);
		break;
	case GuiControlState::PRESSED:
		app->render->DrawTexture(gui_Buttons, buttonX, buttonY, &tex2);
		app->render->DrawTextGUI(text.GetString(), buttonX +45, buttonY + 10, bounds.w - 100, bounds.h - 30);
		break;
	}


	return false;
}

