#pragma once
#include "Module.h"
struct Texture;

class ModuleTextures : public Module
{
public:
	ModuleTextures(bool active);
	~ModuleTextures();

	bool Init(const nlohmann::json::iterator& it);
	bool Start();

	Texture* LoadTexture(const char* path) const;
	Texture* GetDefaultTexture();

private:
	Texture* CreateDefaultTexture() const;
	Texture* default_texture;
};

