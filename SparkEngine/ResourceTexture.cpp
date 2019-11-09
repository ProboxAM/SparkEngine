#include "ResourceTexture.h"

ResourceTexture::~ResourceTexture()
{
}

ResourceTexture::ResourceTexture(uint id, Resource::RESOURCE_TYPE type) : Resource(id, type)
{
}
