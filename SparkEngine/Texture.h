#ifndef _TEXTURE_H_
#define _TEXTURE_H_

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


#endif // !_TEXTURE_H_



