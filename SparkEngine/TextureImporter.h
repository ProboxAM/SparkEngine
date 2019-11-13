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
	bool Start();

	bool Import(const char* file, std::string& output_file, uint id);
	bool Import(const void* buffer, uint size, std::string& output_file);

	bool Load(ResourceTexture* resource);
	void LoadDefault(ResourceTexture* resource);
	bool CreateMeta(std::string file, uint id);

	int version;
	uint checkers;
};


#endif // !_MATERIAL_IMPORTER_H_



