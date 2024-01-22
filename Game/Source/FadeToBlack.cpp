#include "FadeToBlack.h"

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
	return true;
}

bool FadeToBlack::Update(float dt)
{
	return true;
}

bool FadeToBlack::CleanUp()
{
	return true;
}
