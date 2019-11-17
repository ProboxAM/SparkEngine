#ifndef _RESOURCE_TEXTURE_H_
#define _RESOURCE_TEXTURE_H_

#include "Resource.h"

class ResourceTexture : public Resource
{
public:
	struct TextureMetaFile : public MetaFile
	{
		//texture import settings
		enum TEXTURE_COMPRESSION { DXT1, DXT3, DXT5};
		TEXTURE_COMPRESSION compression = DXT5;

		enum TEXTURE_WRAP_MODE { REPEAT, MIRRORED_REPEAT, CLAMP, CLAMP_TO_BORDER };
		TEXTURE_WRAP_MODE wrap_s = REPEAT;
		TEXTURE_WRAP_MODE wrap_t = REPEAT;

		enum TEXTURE_FILTER_MODE { NEAREST, LINEAR };
		TEXTURE_FILTER_MODE min_filter = NEAREST;
		TEXTURE_FILTER_MODE mag_filter = NEAREST;

		int GetCompression();
		int GetWrapModeS();
		int GetWrapModeT();
		int GetMinFilter();
		int GetMagFilter();
	};

public:
	~ResourceTexture();
	ResourceTexture(uint id);

	uint buffer_id;
	uint width, height, depth, bpp, mips;
	int size;
	std::string format;

	void UnLoad();
};


#endif // !_TEXTURE_H_



