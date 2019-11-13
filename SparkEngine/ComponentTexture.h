#ifndef _COMPONENT_TEXTURE_H_
#define _COMPONENT_TEXTURE_H_

struct ResourceTexture;
typedef unsigned int uint;
#include "Component.h"
#include "Globals.h"

class ComponentTexture : public Component
{
public:
	ComponentTexture(GameObject* gameobject);
	~ComponentTexture();

	ResourceTexture* GetTexture();
	void AddTexture(ResourceTexture* tex);
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
	bool Load(const nlohmann::json comp);

private:
	ResourceTexture * tex = nullptr;
public:
	bool debug_texture = false;

};

#endif // !_COMPONENT_TEXTURE_H_




