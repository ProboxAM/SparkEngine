#include "Texture.h"
#include "ModuleTextures.h"

#define ILUT_USE_OPENGL
#include "DeviL/include/ilut.h"

#pragma comment (lib,"DeviL/lib/DevIL.lib")
#pragma comment (lib,"DeviL/lib/ILUT.lib")
#pragma comment (lib,"DeviL/lib/ILU.lib")


ModuleTextures::ModuleTextures(bool active) : Module(active)
{
	name = "Textures";
}


ModuleTextures::~ModuleTextures()
{
}

bool ModuleTextures::Init(nlohmann::json::iterator it)
{
	ilInit();
	iluInit();
	ilutInit();
	ilEnable(IL_CONV_PAL);
	ilutEnable(ILUT_OPENGL_CONV);
	ilutRenderer(ILUT_OPENGL);

	LOG("Initialized DevIL version: %d", (int)ilGetInteger(IL_VERSION_NUM));

	return true;
}

bool ModuleTextures::Start()
{
	default_texture = CreateDefaultTexture();

	return true;
}

Texture* ModuleTextures::CreateDefaultTexture() const
{
	Texture* tex = new Texture();

	tex->path = "CheckersTexture";
	tex->width = 128;
	tex->height = 128;
	GLubyte checkImage[128][128][4];
	for (int i = 0; i < 128; i++)
	{
		for (int j = 0; j < 128; j++)
		{
			int c = ((((i & 0x8) == 0) ^ (((j & 0x8)) == 0))) * 255;
			checkImage[i][j][0] = (GLubyte)c;
			checkImage[i][j][1] = (GLubyte)c;
			checkImage[i][j][2] = (GLubyte)c;
			checkImage[i][j][3] = (GLubyte)255;
		}
	}
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &tex->id);
	glBindTexture(GL_TEXTURE_2D, tex->id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 128, 128, 0, GL_RGBA, GL_UNSIGNED_BYTE, checkImage);
	glBindTexture(GL_TEXTURE_2D, 0);

	tex->mips = 0;
	tex->depth = 0;
	tex->format = "rgba";
	tex->bpp = 0;
	tex->size = sizeof(GLubyte) * 4 * 128 * 128;

	return tex;
}
Texture* ModuleTextures::LoadTexture(const char* path) const
{
	Texture* tex = new Texture();
	uint image_id;

	std::string final_path = ASSETS_FOLDER + std::string(path);

	ilGenImages(1, &image_id); // Grab a new image name.
	ilBindImage(image_id);
	ilLoadImage(final_path.c_str());

	tex->id = ilutGLBindTexImage();
	tex->width = ilGetInteger(IL_IMAGE_WIDTH);
	tex->height = ilGetInteger(IL_IMAGE_HEIGHT);
	tex->path = final_path;
	tex->mips = ilGetInteger(IL_NUM_MIPMAPS);
	tex->depth = ilGetInteger(IL_IMAGE_DEPTH);

	ILenum format = ilGetInteger(IL_IMAGE_FORMAT);
	switch (format)
	{
		case IL_COLOR_INDEX:tex->format = "color index"; break;
		case IL_ALPHA:tex->format = "alpha"; break;
		case IL_RGB:tex->format = "rgb"; break;
		case IL_RGBA:tex->format = "rgba"; break;
		case IL_BGR:tex->format = "bgr"; break;
		case IL_BGRA:tex->format = "bgra"; break;
		case IL_LUMINANCE:tex->format = "luminance"; break;
		case IL_LUMINANCE_ALPHA:tex->format = "luminance alpha"; break;
	}

	tex->bpp = ilGetInteger(IL_IMAGE_BPP);
	tex->size = ilGetInteger(IL_IMAGE_SIZE_OF_DATA);
	glBindTexture(GL_TEXTURE_2D, 0);
	ilDeleteImages(1, &image_id);

	LOG("Loaded Texture %s. width = %i height = %i \nformat = %s size = %i", tex->path.c_str(), tex->width,tex->height,tex->format.c_str(),tex->size);

	return tex;
}

Texture* ModuleTextures::GetDefaultTexture()
{
	return default_texture;
}
