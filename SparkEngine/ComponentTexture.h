#pragma once

struct Texture;
#include "Component.h"
#include "Globals.h"

class ComponentTexture : public Component
{
public:
	ComponentTexture(GameObject* gameobject);
	~ComponentTexture();

	Texture* GetTexture();
	void AddTexture(Texture* tex);
	void SetDebugMode();

	std::string GetTexturePath();
	uint GetTextureWidth();
	uint GetTextureHeight();

private:
	Texture* tex = nullptr;
public:
	bool debug_texture = false;

};

