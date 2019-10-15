#pragma once

#include <string>
typedef unsigned int uint;

struct Texture
{
public:
	uint id;
	uint width, height, depth, bpp, mips;
	int size;
	std::string path, format;
};

