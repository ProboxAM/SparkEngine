#include "ResourceTexture.h"

ResourceTexture::~ResourceTexture()
{
}

ResourceTexture::ResourceTexture(uint id) : Resource(id, Resource::RESOURCE_TYPE::R_TEXTURE)
{
}
