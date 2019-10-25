#ifndef _MODULE_IMPORTER_H_
#define _MODULE_IMPORTER_H_

#include <vector>
#include "Module.h"

class Mesh;
struct Texture;
class aiMesh;
class aiNode;
class aiScene;
class GameObject;

class ModuleImporter : public Module
{
public:
	ModuleImporter(bool start_enabled = true);
	~ModuleImporter();

	bool Init(const nlohmann::json::iterator& it);
	update_status PreUpdate(float dt);
	bool CleanUp();
	void LoadFBXFile(const char* file);
	void ImportFile(const char* path);

private:
	void LoadNode(const aiNode* node, const aiScene* scene, GameObject* parent);
	void FixScaleUnits(float3 &scale);
};

#endif