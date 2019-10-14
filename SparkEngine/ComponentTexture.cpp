#include "Application.h"
#include "ModuleTextures.h"
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
