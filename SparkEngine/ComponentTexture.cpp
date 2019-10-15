#include "Application.h"
#include "ModuleTextures.h"
#include "Texture.h"
#include "ComponentTexture.h"




ComponentTexture::ComponentTexture(GameObject* gameobject):Component(gameobject)
{

}


ComponentTexture::~ComponentTexture()
{
}

Texture* ComponentTexture::GetTexture()
{
	if(debug_texture)
		return App->textures->GetDefaultTexture();
	else
		return tex;
}

void ComponentTexture::AddTexture(Texture * tex)
{
	this->tex = tex;
}

void ComponentTexture::SetDebugMode()
{
	debug_texture = !debug_texture;
}

std::string ComponentTexture::GetTexturePath()
{
	return tex->path;
}
std::string ComponentTexture::GetTextureFormat()
{
	return tex->format;
}
float ComponentTexture::GetTextureSize()
{
	return tex->size;
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
