#ifndef _COMPONENT_TEXTURE_H_
#define _COMPONENT_TEXTURE_H_

struct Texture;
typedef unsigned int uint;
#include "Component.h"
#include "Globals.h"

class ComponentTexture : public Component
{
public:
	ComponentTexture(GameObject* gameobject);
	~ComponentTexture();

	Texture* GetTexture();
	void AddTexture(Texture* tex);
	void SetDebugMode();

	std::string GetTexturePath();
	std::string GetTextureFormat();
	int GetTextureSize();
	int GetTextureID();
	uint GetTextureWidth();
	uint GetTextureHeight();
	uint GetTextureBPP();
	uint GetTextureMIP();
	uint GetTextureDepth();

	bool Save(const nlohmann::json::iterator& it);

private:
	Texture * tex = nullptr;
public:
	bool debug_texture = false;

};

#endif // !_COMPONENT_TEXTURE_H_




