#ifndef _RESOURCE_TEXTURE_H_
#define _RESOURCE_TEXTURE_H_

#include "Resource.h"

struct ResourceTexture : public Resource
{
public:
	uint width, height, depth, bpp, mips;
	int size;
	std::string format;
};


#endif // !_TEXTURE_H_



