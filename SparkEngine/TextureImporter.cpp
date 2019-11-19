#include "Application.h"
#include "ModuleFileSystem.h"
#include "ModuleResources.h"
#include "ResourceTexture.h"

#include "glew\glew.h"
#define ILUT_USE_OPENGL
#include "DeviL/include/ilut.h"

#include "TextureImporter.h"

#include <fstream>
#include <iomanip>

#pragma comment (lib,"DeviL/lib/DevIL.lib")
#pragma comment (lib,"DeviL/lib/ILUT.lib")
#pragma comment (lib,"DeviL/lib/ILU.lib")

#define CHECKERS_ID 4294967295

TextureImporter::TextureImporter()
{
}


TextureImporter::~TextureImporter()
{
}

bool TextureImporter::Init()
{
	ilInit();
	iluInit();
	ilutInit();
	ilEnable(IL_CONV_PAL);
	ilutEnable(ILUT_OPENGL_CONV);
	ilutRenderer(ILUT_OPENGL);

	version = ilGetInteger(IL_VERSION_NUM);
	LOG("Initialized DevIL version: %i", version);

	checkers = CHECKERS_ID;

	return true;
}

bool TextureImporter::Start()
{
	ResourceTexture* texture = (ResourceTexture*)App->resources->CreateResource(Resource::RESOURCE_TYPE::R_TEXTURE, CHECKERS_ID);
	LoadDefault(texture);
	texture->AddReference();
	texture->SetFile("Checkers");

	return true;
}

bool TextureImporter::CleanUp()
{
	return true;
}

bool TextureImporter::Load(ResourceTexture* tex)
{
	uint image_id;

	ResourceTexture::TextureMetaFile* meta = (ResourceTexture::TextureMetaFile*) tex->meta;
	ilGenImages(1, &image_id); // Grab a new image name.
	ilBindImage(image_id);
	if (ilLoadImage(tex->GetExportedFile()))
	{
		tex->buffer_id = ilutGLBindTexImage();

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, meta->GetWrapModeS());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, meta->GetWrapModeT());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, meta->GetMagFilter());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, meta->GetMinFilter());

		tex->width = ilGetInteger(IL_IMAGE_WIDTH);
		tex->height = ilGetInteger(IL_IMAGE_HEIGHT);
		tex->mips = ilGetInteger(IL_NUM_MIPMAPS);
		tex->depth = ilGetInteger(IL_IMAGE_DEPTH);

		ILenum format = ilGetInteger(IL_IMAGE_FORMAT);
		switch (format)
		{
			case IL_COLOR_INDEX:tex->format = "color index"; break;
			case IL_ALPHA:tex->format = "alpha"; break;
			case IL_RGB:tex->format = "rgb"; break;
			case IL_RGBA:tex->format = "rgba"; break;
			case IL_BGR:tex->format = "bgr"; break;
			case IL_BGRA:tex->format = "bgra"; break;
			case IL_LUMINANCE:tex->format = "luminance"; break;
			case IL_LUMINANCE_ALPHA:tex->format = "luminance alpha"; break;
		}

		tex->bpp = ilGetInteger(IL_IMAGE_BPP);
		tex->size = ilGetInteger(IL_IMAGE_SIZE_OF_DATA);

		glBindTexture(GL_TEXTURE_2D, 0);

		LOG("Loaded Texture %s", tex->GetExportedFile());
	}
	else
	{
		tex = (ResourceTexture*) App->resources->GetAndLoad(checkers);
		LOG("Error loading texture. Applied default texture instead.");
	}
	ilDeleteImages(1, &image_id);

	return tex;
}

bool TextureImporter::Import(const void * buffer, uint size, std::string & output_file)
{
	return true;
}

bool TextureImporter::Import(const char* import_file, std::string& output_file, ResourceTexture::TextureMetaFile*& meta)
{
	bool ret = false;

	uint image_id;

	if (ilLoadImage(import_file))
	{
		ilEnable(IL_FILE_OVERWRITE);
		ILuint size;
		ILubyte *data;

		ilSetInteger(IL_DXTC_FORMAT, meta ? meta->GetCompression() : IL_DXT5); // To pick a specific DXT compression use 

		size = ilSaveL(IL_DDS, NULL, 0); // Get the size of the data buffer 
		if (size > 0) {
			data = new ILubyte[size]; // allocate data buffer 
			if (ilSaveL(IL_DDS, data, size) > 0) // Save to buffer with the ilSaveIL function        
			{
				if (!meta) //If there was no meta, we create a new one for this resource and generate id.
				{
					ResourceTexture::TextureMetaFile* new_meta_file = new ResourceTexture::TextureMetaFile();
					meta = new_meta_file;
					meta->id = App->GenerateID();
				}				

				std::string file_name = std::to_string(meta->id);
				output_file = LIBRARY_TEXTURES_FOLDER + file_name + TEXTURE_EXTENSION;
				ret = App->fsystem->Save(output_file.c_str(), data, size);

				App->fsystem->GetFileModificationDate(import_file, meta->modification_date);
				meta->exported_file = output_file;
				meta->original_file = import_file;
				meta->file = std::string(import_file) + ".meta";
				SaveMeta(meta);
			}
			RELEASE_ARRAY(data);
		}
		ilDeleteImages(1, &image_id);
	}

	return ret;
}

void TextureImporter::LoadDefault(ResourceTexture* resource)
{
	resource->width = 128;
	resource->height = 128;
	GLubyte checkImage[128][128][4];

	for (int i = 0; i < 128; i++)
	{
		for (int j = 0; j < 128; j++)
		{
			int c = ((((i & 0x8) == 0) ^ (((j & 0x8)) == 0))) * 255;
			checkImage[i][j][0] = (GLubyte)c;
			checkImage[i][j][1] = (GLubyte)c;
			checkImage[i][j][2] = (GLubyte)c;
			checkImage[i][j][3] = (GLubyte)255;
		}
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &resource->buffer_id);
	glBindTexture(GL_TEXTURE_2D, resource->buffer_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 128, 128, 0, GL_RGBA, GL_UNSIGNED_BYTE, checkImage);
	glBindTexture(GL_TEXTURE_2D, 0);

	resource->mips = 0;
	resource->depth = 0;
	resource->format = "rgba";
	resource->bpp = 0;
	resource->size = sizeof(GLubyte) * 4 * 128 * 128;
}

bool TextureImporter::LoadMeta(const char * file, ResourceTexture::TextureMetaFile* meta)
{
	std::ifstream i(file);
	nlohmann::json json = nlohmann::json::parse(i);

	meta->file = file;
	meta->exported_file = json["exported_file"].get<std::string>();
	meta->original_file = json["original_file"].get<std::string>();
	meta->id = json["id"];
	meta->modification_date = json["modification_date"].get<std::string>();

	meta->min_filter = json["min_filter"];
	meta->mag_filter = json["mag_filter"];
	meta->wrap_s = json["wrap_s"];
	meta->wrap_t = json["wrap_t"];
	meta->compression = json["compression"];

	meta->loaded = true;

	return true;
}

bool TextureImporter::SaveMeta(ResourceTexture::TextureMetaFile* meta)
{
	nlohmann::json meta_file;
	meta_file = {
		{ "original_file", meta->original_file },
		{ "exported_file", meta->exported_file },
		{ "id", meta->id },
		{ "modification_date", meta->modification_date},
		{ "min_filter", meta->min_filter },
		{ "mag_filter", meta->mag_filter },
		{ "wrap_s", meta->wrap_s },
		{ "wrap_t", meta->wrap_t },
		{ "compression", meta->compression }
	};

	std::ofstream o(meta->file);
	o << std::setw(4) << meta_file << std::endl;

	return true;
}
