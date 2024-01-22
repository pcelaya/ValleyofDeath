#ifndef __FADETOBLACK_H__
#define __FADETOBLACK_H__

#include "Module.h"

class FadeToBlack : public Module
{
public:
	
	FadeToBlack();

	// Destructor
	virtual ~FadeToBlack();

	// Called before render is available
	bool Awake(pugi::xml_node config);

	// Called after Awake
	bool Start();

	// Called every frame
	bool Update(float dt);

	// Called before quitting
	bool CleanUp();

private:
	pugi::xml_node config;
};

#endif // __ENTITYMANAGER_H__
