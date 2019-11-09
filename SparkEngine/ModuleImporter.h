#ifndef _MODULE_IMPORTER_H_
#define _MODULE_IMPORTER_H_

#include <vector>
#include "Module.h"

class MeshImporter;
class TextureImporter;
class SceneImporter;

class ModuleImporter : public Module
{
public:
	ModuleImporter(bool start_enabled = true);
	~ModuleImporter();

	bool Init(const nlohmann::json::iterator& it);
	bool CleanUp();
	void ImportFile(const char* path);

public:
	MeshImporter* mesh;
	TextureImporter* texture;
	SceneImporter* scene;
};

#endif