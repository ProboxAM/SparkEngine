#ifndef _TEXTURE_IMPORTER_H_
#define _TEXTURE_IMPORTER_H_

#include "Importer.h"

struct ResourceTexture;

class TextureImporter :
	public Importer
{
public:
	TextureImporter();
	~TextureImporter();

	bool Init();
	bool CleanUp();

	bool Import(const char* file, std::string& output_file, uint id);
	bool Import(const void* buffer, uint size, std::string& output_file);

	bool Load(ResourceTexture* resource);
	ResourceTexture* LoadDefault();
	bool CreateMeta(std::string file, uint id);

	int version;
private:

	ResourceTexture* default_texture = nullptr;
};


#endif // !_MATERIAL_IMPORTER_H_



