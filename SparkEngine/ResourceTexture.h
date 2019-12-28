#ifndef _RESOURCE_TEXTURE_H_
#define _RESOURCE_TEXTURE_H_

#include "Resource.h"

class ResourceTexture : public Resource
{
public:
	~ResourceTexture();
	ResourceTexture(uint id);

	uint buffer_id;
	uint width, height, depth, bpp, mips;
	int size;
	std::string format;

	void UnLoad();
	void ReLoad();
	void Load();
	std::string GetTypeString() const;
};


#endif // !_TEXTURE_H_



