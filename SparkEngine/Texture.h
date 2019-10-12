#pragma once

#include "Globals.h"

class aiMaterial;
enum aiTextureType;

struct Texture
{
public:
	void LoadTexture(aiMaterial* mat, aiTextureType type);

public:
	uint id;
	uint width, height;
	std::string path;
};

