#ifndef _MODULE_IMPORTER_H_
#define _MODULE_IMPORTER_H_

#include <vector>
#include "Module.h"

class Mesh;
class aiMesh;
class aiScene;

typedef void(*aiLogStreamCallback)(const char*, char*);

class ModuleImporter : public Module
{
public:
	ModuleImporter(bool start_enabled = true);
	~ModuleImporter();

	bool Init(nlohmann::json::iterator it);
	update_status PreUpdate(float dt);
	bool CleanUp();
	std::vector<Mesh> LoadFBXFile(const char* file);

private:
	Mesh LoadMesh(const aiScene* scene, aiMesh* mesh);
	aiLogStreamCallback callback;
};

#endif