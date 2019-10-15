#pragma once

struct Texture;
typedef unsigned int uint;
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
	std::string GetTextureFormat();
	int GetTextureSize();
	uint GetTextureWidth();
	uint GetTextureHeight();
	uint GetTextureBPP();
	uint GetTextureMIP();
	uint GetTextureDepth();

private:
	Texture* tex = nullptr;
public:
	bool debug_texture = false;

};

