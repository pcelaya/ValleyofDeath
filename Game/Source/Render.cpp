#include "App.h"
#include "Window.h"
#include "Render.h"
#include "Map.h"
#include "Physics.h"

#include "Defs.h"
#include "Log.h"


#define VSYNC true

Render::Render() : Module()
{
	name.Create("renderer");
	background.r = 0;
	background.g = 0;
	background.b = 0;
	background.a = 0;
}

// Destructor
Render::~Render()
{}

// Called before render is available
bool Render::Awake(pugi::xml_node config)
{
	LOG("Create SDL rendering context");
	bool ret = true;

	Uint32 flags = SDL_RENDERER_ACCELERATED;

	// L04: DONE 6: Load the VSYNC status from config.xml and adapt the code to set it on / off
	if (config.child("vsync").attribute("value").as_bool()) {
		flags |= SDL_RENDERER_PRESENTVSYNC;
		LOG("Using vsync");
	}
	else {
		LOG("vsync OFF");
	}
	 
	renderer = SDL_CreateRenderer(app->win->window, -1, flags);

	if(renderer == NULL)
	{
		LOG("Could not create the renderer! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	else
	{
		camera.w = app->win->screenSurface->w;
		camera.h = app->win->screenSurface->h;
		camera.x = 0;
		camera.y = 0;
	}

	return ret;
}

// Called before the first frame
bool Render::Start()
{
	LOG("render start");
	// back background
	SDL_RenderGetViewport(renderer, &viewport);
	return true;
}

// Called each loop iteration
bool Render::PreUpdate()
{
	SDL_RenderClear(renderer);
	return true;
}

bool Render::Update(float dt)
{
	return true;
}

bool Render::PostUpdate()
{
	SDL_SetRenderDrawColor(renderer, background.r, background.g, background.b, background.a);
	SDL_RenderPresent(renderer);
	return true;
}

// Called before quitting
bool Render::CleanUp()
{
	LOG("Destroying SDL render");
	SDL_DestroyRenderer(renderer);
	return true;
}

void Render::SetBackgroundColor(SDL_Color color)
{
	background = color;
}

void Render::SetViewPort(const SDL_Rect& rect)
{
	SDL_RenderSetViewport(renderer, &rect);
}

void Render::ResetViewPort()
{
	SDL_RenderSetViewport(renderer, &viewport);
}

// Blit to screen
bool Render::DrawTexture(SDL_Texture* texture, int x, int y, const SDL_Rect* section, float speed, double angle, int pivotX, int pivotY) const
{
	bool ret = true;
	uint scale = app->win->GetScale();

	SDL_Rect rect;
	rect.x = (int)(camera.x * speed) + x * scale;
	rect.y = (int)(camera.y * speed) + y * scale;

	if(section != NULL)
	{
		rect.w = section->w;
		rect.h = section->h;
	}
	else
	{
		SDL_QueryTexture(texture, NULL, NULL, &rect.w, &rect.h);
	}

	rect.w *= scale;
	rect.h *= scale;

	SDL_Point* p = NULL;
	SDL_Point pivot;

	if(pivotX != INT_MAX && pivotY != INT_MAX)
	{
		pivot.x = pivotX;
		pivot.y = pivotY;
		p = &pivot;
	}

	if(SDL_RenderCopyEx(renderer, texture, section, &rect, angle, p, SDL_FLIP_NONE) != 0)
	{
		LOG("Cannot blit to screen. SDL_RenderCopy error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool Render::DrawTexturePR(SDL_Texture* texture, int x, int y, const SDL_Rect* section, float speed, double angle, int pivotX, int pivotY) const
{
	bool ret = true;
	uint scale = app->win->GetScale();

	SDL_Rect rect;
	rect.x = (int)(camera.x * speed) + x * scale;
	rect.y = (int)(camera.y * speed) + y * scale;

	if (section != NULL)
	{
		rect.w = section->w;
		rect.h = section->h;
	}
	else
	{
		SDL_QueryTexture(texture, NULL, NULL, &rect.w, &rect.h);
	}

	rect.w *= scale;
	rect.h *= scale;

	SDL_Point* p = NULL;
	SDL_Point pivot;

	if (pivotX != INT_MAX && pivotY != INT_MAX)
	{
		pivot.x = pivotX;
		pivot.y = pivotY;
		p = &pivot;
	}

	if (SDL_RenderCopyEx(renderer, texture, section, &rect, angle, p, SDL_FLIP_HORIZONTAL) != 0)
	{
		LOG("Cannot blit to screen. SDL_RenderCopy error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool Render::DrawRectangle(const SDL_Rect& rect, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool filled, bool use_camera) const
{
	bool ret = true;
	uint scale = app->win->GetScale();

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	SDL_Rect rec(rect);
	if(use_camera)
	{
		rec.x = (int)(camera.x + rect.x * scale);
		rec.y = (int)(camera.y + rect.y * scale);
		rec.w *= scale;
		rec.h *= scale;
	}

	int result = (filled) ? SDL_RenderFillRect(renderer, &rec) : SDL_RenderDrawRect(renderer, &rec);

	if(result != 0)
	{
		LOG("Cannot draw quad to screen. SDL_RenderFillRect error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool Render::DrawLine(int x1, int y1, int x2, int y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool use_camera) const
{
	bool ret = true;
	uint scale = app->win->GetScale();

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	int result = -1;

	if(use_camera)
		result = SDL_RenderDrawLine(renderer, camera.x + x1 * scale, camera.y + y1 * scale, camera.x + x2 * scale, camera.y + y2 * scale);
	else
		result = SDL_RenderDrawLine(renderer, x1 * scale, y1 * scale, x2 * scale, y2 * scale);

	if(result != 0)
	{
		LOG("Cannot draw quad to screen. SDL_RenderFillRect error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool Render::DrawCircle(int x, int y, int radius, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool use_camera) const
{
	bool ret = true;
	uint scale = app->win->GetScale();

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	int result = -1;
	SDL_Point points[360];

	float factor = (float)M_PI / 180.0f;

	for(uint i = 0; i < 360; ++i)
	{
		points[i].x = (int)(x * scale + camera.x) + (radius * cos(i * factor));
		points[i].y = (int)(y * scale + camera.y) + (radius * sin(i * factor));
	}

	result = SDL_RenderDrawPoints(renderer, points, 360);

	if(result != 0)
	{
		LOG("Cannot draw quad to screen. SDL_RenderFillRect error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}


SDL_Texture* Render::LoadTexture(SDL_Renderer* renderer, const char* filePath)
{
	SDL_Surface* surface = IMG_Load(filePath);  // Carga una imagen usando SDL_image
	if (!surface) {
		// Manejar errores de carga de imagen
		return nullptr;
	}

	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);  // Convierte la superficie en una textura
	SDL_FreeSurface(surface);  // Libera la superficie, ya que no la necesitas más

	if (!texture) 
	{
		// Manejar errores de creación de textura
		return nullptr;
	}

	return texture;
}

bool Render::LoadState(pugi::xml_node save)
{
	if (app->render->camera.x < 0)
		camera.x = save.child("camera").attribute("x").as_int();

	if (app->render->camera.y < 0)
		camera.y = save.child("camera").attribute("y").as_int();

	//Iterates over the entities and changes its attributes
	ListItem<Entity*>* item = app->entityManager->entities.start;

	for (pugi::xml_node configEntity = config.first_child(); configEntity && item != NULL; configEntity = configEntity.next_sibling())
	{
		pugi::xml_node newEntity = save.parent().child("scene").child(configEntity.name());
		
		if (item->data->active)
		{
			b2Vec2 savedPos = b2Vec2(METERS_TO_PIXELS(newEntity.attribute("x").as_int()), METERS_TO_PIXELS(newEntity.attribute("y").as_int()));
			item->data->pbody->body->SetTransform(savedPos, 0);
			item->data->position = savedPosition;
		}

		item->data->active = newEntity.attribute("active").as_bool();
		item = item->next;
	}

	return true;
}

bool Render::SaveState(pugi::xml_node save)
{
	//Saves camera position
	pugi::xml_node camNode = save.parent().child("renderer").append_child("camera");
	camNode.append_attribute("x").set_value(camera.x);
	camNode.append_attribute("y").set_value(camera.y);

	//Iterates over the entities and saves its attributes
	ListItem<Entity*>* item = app->entityManager->entities.start;

	for (pugi::xml_node configEntity = config.first_child(); configEntity && item != NULL; configEntity = configEntity.next_sibling())
	{
		pugi::xml_node newEntity = save.parent().child("scene").append_child(configEntity.name());

		if (item->data->active)
		{
			newEntity.append_attribute("x").set_value(METERS_TO_PIXELS(item->data->position.x));
			newEntity.append_attribute("y").set_value(METERS_TO_PIXELS(item->data->position.y));
			savedPosition = iPoint(newEntity.attribute("x").as_int(), newEntity.attribute("y").as_int());
		}

		newEntity.append_attribute("active").set_value(item->data->active);
		item = item->next;
	}

	return true;
}

int Render::GetFirstTileX()
{
	return (camera.x * -1) / app->map->GetTileWidth();
}

int Render::GetLastTileX()
{
	return ((camera.x * -1) + camera.w + app->map->GetTileWidth()) / app->map->GetTileWidth();
}

int Render::GetFirstTileY()
{
	return (camera.y * -1) / app->map->GetTileHeight();
}

int Render::GetLastTileY()
{
	return ((camera.y * -1) + camera.h + app->map->GetTileHeight()) / app->map->GetTileHeight();
}