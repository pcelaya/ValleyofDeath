#include "FadeToBlack.h"
#include "App.h"
#include "Intro.h"
//#include "Menu.h"
#include "Level1.h"

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
	SDL_SetRenderDrawBlendMode(app->render->renderer, SDL_BLENDMODE_BLEND);
	currentStep = Fade_Step::TO_BLACK;
	
	return true;
}

bool FadeToBlack::Update(float dt)
{
	if (fadetoblack)
	{
		FadeBlack();
	}
	return true;
}

bool FadeToBlack::CleanUp()
{
	return true;
}

void FadeToBlack::NextScene()
{
	switch (typescene)
	{
	case TypeScene::INTRO:
		app->intro->CleanUp();
		app->level_1->Init();
		app->level_1->Start();
		typescene = TypeScene::LEVEL_1;
		break;

	case TypeScene::LEVEL_1:
		app->level_1->CleanUp();
		app->intro->Init();
		app->intro->Start();
		typescene = TypeScene::INTRO;
		break;

	case TypeScene::NONE:
		break;

	default:
		break;
	}
}

void FadeToBlack::FadeBlack()
{
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
}
