#pragma once
#include "Module.h"
struct Texture;

class ModuleTextures : public Module
{
public:
	ModuleTextures(bool active);
	~ModuleTextures();

	bool Init(nlohmann::json::iterator it);
	bool Start();

	Texture* LoadTexture(const char* path);
	Texture* GetDefaultTexture();

private:
	Texture* CreateDefaultTexture();
	Texture* default_texture;
};

