#pragma once

#include "Globals.h"

class aiMaterial;
enum aiTextureType;

class Texture
{
public:
	Texture();
	~Texture();

	void LoadTexture(aiMaterial* mat, aiTextureType type);

public:
	uint id;
};

