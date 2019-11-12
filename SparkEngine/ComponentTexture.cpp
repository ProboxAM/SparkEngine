#include "Application.h"
#include "ModuleImporter.h"
#include "ResourceTexture.h"
#include "Importer.h"
#include "TextureImporter.h"
#include "ComponentTexture.h"




ComponentTexture::ComponentTexture(GameObject* gameobject):Component(gameobject)
{

}


ComponentTexture::~ComponentTexture()
{
}

ResourceTexture* ComponentTexture::GetTexture()
{
	if(debug_texture)
		return App->importer->texture->LoadDefault();
	else
		return tex;
}

void ComponentTexture::AddTexture(ResourceTexture * tex)
{
	this->tex = tex;
}

void ComponentTexture::SetDebugMode()
{
	debug_texture = !debug_texture;
}

std::string ComponentTexture::GetTexturePath()
{
	return this->tex->GetFile();
}
std::string ComponentTexture::GetTextureFormat()
{
	return tex->format;
}
int ComponentTexture::GetTextureSize()
{
	return tex->size;
}
int ComponentTexture::GetTextureID()
{
	return GetTexture()->buffer_id;
}
uint ComponentTexture::GetTextureWidth()
{
	return tex->width;
}
uint ComponentTexture::GetTextureHeight()
{
	return tex->height;
}

uint ComponentTexture::GetTextureBPP()
{
	return tex->bpp;
}

uint ComponentTexture::GetTextureMIP()
{
	return tex->mips;
}

uint ComponentTexture::GetTextureDepth()
{
	return tex->depth;
}

bool ComponentTexture::Save(const nlohmann::json::iterator & it)
{
	nlohmann::json object = {
		{"active",active},
		{"debug_tex", debug_texture},
		{"type",type},
		{"texture", GetTexturePath() },
	};

	it.value().push_back(object);

	return true;
}
