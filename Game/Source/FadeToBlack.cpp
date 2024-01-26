#include "FadeToBlack.h"
#include "App.h"
#include "Intro.h"
#include "Menu.h"
#include "InGameMenu.h"
#include "Level1.h"
#include "Settings.h"
#include "Map.h"
#include "Window.h"

FadeToBlack::FadeToBlack()
{
	name.Create("fade_to_black");
}

FadeToBlack::~FadeToBlack(){}

bool FadeToBlack::Awake(pugi::xml_node config)
{
	this->config = config;
	return true;
}

bool FadeToBlack::Start()
{
	app->win->GetWindowSize(screen_width, screen_height);
	screenRect = { 0, 0, (int)screen_width, (int)screen_height };

	SDL_SetRenderDrawBlendMode(app->render->renderer, SDL_BLENDMODE_BLEND);
	
	return true;
}

bool FadeToBlack::Update(float dt)
{
	if (currentStep == Fade_Step::NO)
		return true;

	if (currentStep == Fade_Step::TO_BLACK)
	{
		++frameCount;
		if (frameCount >= maxFadeFrames)
		{
			NextScene();

			currentStep = Fade_Step::FROM_BLACK;

			if (maxFadeFramesBack != 0)
			{
				frameCount = maxFadeFramesBack;
				maxFadeFrames = maxFadeFramesBack;
			}
		}
	}
	else
	{
		--frameCount;
		if (frameCount <= 0)
		{
			currentStep = Fade_Step::NO;
			fadetoblack = false;
			maxFadeFrames = 0;
		}
	}

	return true;
}

bool FadeToBlack::PostUpdate()
{
	if (currentStep == Fade_Step::NO) return true;      
	float fadeRatio = (float)frameCount / (float)maxFadeFrames;         
	
	SDL_SetRenderDrawColor(app->render->renderer, 0, 0, 0, (Uint8)(fadeRatio * 255.0f));     
	SDL_RenderFillRect(app->render->renderer, &screenRect);      
	
	return true;
}

bool FadeToBlack::CleanUp()
{
	active = false;
	
	return true;
}

void FadeToBlack::NextScene()
{
	switch (typescene)
	{
	case TypeScene::INTRO:
		app->intro->CleanUp();

		app->menu->Init();
		app->settings->Init();

		app->menu->Start();

		typescene = TypeScene::MENU;
		break;

	case TypeScene::MENU:
		app->menu->CleanUp();

		app->level_1->Init();
		app->map->Init();
		app->entityManager->Init();
		app->ingame_menu->Init();
		app->settings->Init();

		app->level_1->Start();

		typescene = TypeScene::LEVEL_1;
		break;

	case TypeScene::LEVEL_1:
		app->level_1->CleanUp();

		moduleToEnable->Init();
		moduleToEnable->Awake(config.parent().child("level_2"));
		moduleToEnable->Start();
		app->entityManager->Start();
		app->AddModule(moduleToEnable);

		if (goBack)
			typescene = TypeScene::MENU;
		else
		{
			typescene = TypeScene::LEVEL_2;
		}
		break;

	case TypeScene::LEVEL_2:
		moduleToDisable->CleanUp();

		moduleToEnable->Init();
		moduleToEnable->Start();

		typescene = TypeScene::END;
		break;

	case TypeScene::NONE:
		break;

	default:
		break;
	}
}

bool FadeToBlack::FadeBlack(Module* toDisable, Module* toEnable, float frames, bool goBack)
{
	bool ret = false;

	this->goBack = goBack;

	if (currentStep == Fade_Step::NO)
	{
		currentStep = Fade_Step::TO_BLACK;
		frameCount = 0;
		maxFadeFrames = frames;

		this->moduleToDisable = toDisable;
		this->moduleToEnable = toEnable;

		ret = true;
	}

	return ret;
}
