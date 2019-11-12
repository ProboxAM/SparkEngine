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
	if (buffer_id != 0)
	{
		glDeleteTextures(1, &buffer_id);
		buffer_id = 0;
	}
}
