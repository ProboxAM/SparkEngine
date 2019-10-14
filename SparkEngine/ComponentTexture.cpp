#include "ComponentTexture.h"




ComponentTexture::ComponentTexture(GameObject* gameobject):Component(gameobject)
{

}


ComponentTexture::~ComponentTexture()
{
}

Texture* ComponentTexture::GetTexture()
{
	return tex;
}

void ComponentTexture::AddTexture(Texture * tex)
{
	this->tex = tex;
}
