#include "Application.h"
#include "ModuleImporter.h"
#include "ModuleResources.h"
#include "ResourceTexture.h"
#include "Importer.h"
#include "TextureImporter.h"
#include "ComponentTexture.h"




ComponentTexture::ComponentTexture(GameObject* gameobject):Component(gameobject)
{

}


ComponentTexture::~ComponentTexture()
{
	tex->RemoveReference();
	tex = nullptr;
}

ResourceTexture* ComponentTexture::GetTexture()
{
	if(debug_texture)
		return (ResourceTexture*) App->resources->Get(App->importer->texture->checkers);
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
		{"resource", tex->GetID()},
		{"type",type},
	};

	it.value().push_back(object);

	return true;
}

bool ComponentTexture::Load(const nlohmann::json comp)
{
	active = comp["active"];
	type = comp["type"];
	tex = (ResourceTexture*)App->resources->GetAndReference(comp["resource"]);
	debug_texture = comp["debug_tex"];

	return true;
}
