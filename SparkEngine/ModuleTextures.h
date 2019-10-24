#ifndef _MODULE_TEXTURES_H_
#define _MODULE_TEXTURES_H_

#include "Module.h"
struct Texture;

class ModuleTextures : public Module
{
public:
	ModuleTextures(bool active);
	~ModuleTextures();

	bool Init(const nlohmann::json::iterator& it);
	bool Start();

	Texture* LoadTexture(const char* path);
	Texture* GetDefaultTexture();

	int version;

private:
	Texture * CreateDefaultTexture() const;
	Texture* default_texture;
};


#endif // !_MODULE_TEXTURES_H_