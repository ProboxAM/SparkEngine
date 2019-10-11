#include "Assimp/include/scene.h"

#include "DeviL/include/il.h"
#define ILUT_USE_OPENGL
#include "DeviL/include/ilut.h"
#include "Texture.h"

Texture::Texture()
{
}


Texture::~Texture()
{
}

void Texture::LoadTexture(aiMaterial * mat, aiTextureType type)
{
	aiString str;
	ILuint idd;

	mat->GetTexture(type, 0, &str);
	ilGenImages(1, &idd); // Grab a new image name.
	ilBindImage(idd);
	ilLoadImage(str.C_Str());
	id = ilutGLBindTexImage();
	glBindTexture(GL_TEXTURE_2D, 0);
	ilDeleteImages(1, &idd);
}
