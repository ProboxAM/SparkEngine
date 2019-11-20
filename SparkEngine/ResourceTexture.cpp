#include "Application.h"
#include "ModuleImporter.h"
#include "TextureImporter.h"
#include "ResourceTexture.h"
#include "glew\glew.h"

#define ILUT_USE_OPENGL
#include "DeviL/include/ilut.h"

ResourceTexture::~ResourceTexture()
{
}

ResourceTexture::ResourceTexture(uint id) : Resource(id, Resource::RESOURCE_TYPE::R_TEXTURE)
{
}

void ResourceTexture::UnLoad()
{
	LOG("Unloading Texture %s", exported_file.c_str());
	if (buffer_id != 0)
	{
		glDeleteTextures(1, &buffer_id);
		buffer_id = 0;
	}
}

std::string ResourceTexture::GetTypeString()
{
	return "Texture";
}

int ResourceTexture::TextureMetaFile::GetCompression()
{
	int ret;
	switch (compression)
	{
		case DXT1:
			ret = IL_DXT1;
			break; 
		case DXT3: 
			ret = IL_DXT3;
			break; 
		case DXT5: 
			ret = IL_DXT5;
			break;
	}

	return ret;
}

int ResourceTexture::TextureMetaFile::GetWrapModeS()
{
	int ret;
	switch (wrap_s)
	{
		case REPEAT: 
			ret = GL_REPEAT;
			break;
		case MIRRORED_REPEAT: 
			ret = GL_MIRRORED_REPEAT;
			break;
		case CLAMP: 
			ret = GL_CLAMP;
			break;
		case CLAMP_TO_BORDER: 
			ret = GL_CLAMP_TO_BORDER;
			break;
	}

	return ret;
}

int ResourceTexture::TextureMetaFile::GetWrapModeT()
{
	int ret;
	switch (wrap_t)
	{
	case REPEAT:
		ret = GL_REPEAT;
		break;
	case MIRRORED_REPEAT:
		ret = GL_MIRRORED_REPEAT;
		break;
	case CLAMP:
		ret = GL_CLAMP;
		break;
	case CLAMP_TO_BORDER:
		ret = GL_CLAMP_TO_BORDER;
		break;
	}

	return ret;
}

int ResourceTexture::TextureMetaFile::GetMinFilter()
{
	int ret;
	switch (min_filter)
	{
	case NEAREST:
		ret = GL_NEAREST;
		break;
	case LINEAR:
		ret = GL_LINEAR;
		break;
	}

	return ret;
}

int ResourceTexture::TextureMetaFile::GetMagFilter()
{
	int ret;
	switch (mag_filter)
	{
	case NEAREST:
		ret = GL_NEAREST;
		break;
	case LINEAR:
		ret = GL_LINEAR;
		break;
	}

	return ret;
}

void ResourceTexture::ReLoad()
{
	UnLoad();
	App->importer->texture->Load(this);
}

void ResourceTexture::Load()
{
	App->importer->texture->Load(this);
}
