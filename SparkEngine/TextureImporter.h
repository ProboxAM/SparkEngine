#ifndef _TEXTURE_IMPORTER_H_
#define _TEXTURE_IMPORTER_H_

#include "Importer.h"

struct Texture;

class TextureImporter :
	public Importer
{
public:
	TextureImporter();
	~TextureImporter();

	bool Init();
	bool CleanUp();

	bool Import(const char* file, std::string& output_file);
	bool Import(const void* buffer, uint size, std::string& output_file);

	Texture* Load(const char* exported_file);
	Texture* LoadDefault();

	int version;
private:

	Texture* default_texture;
};


#endif // !_MATERIAL_IMPORTER_H_



