#include "Application.h"
#include "ModuleImporter.h"
#include "TextureImporter.h"
#include "ResourceTexture.h"
#include "glew\glew.h"

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

std::string ResourceTexture::GetTypeString() const
{
	return "Texture";
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
