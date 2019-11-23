#ifndef _MODULE_IMPORTER_H_
#define _MODULE_IMPORTER_H_

#include <vector>
#include "Module.h"

class MeshImporter;
class TextureImporter;
class ModelImporter;

class ModuleImporter : public Module
{
public:
	ModuleImporter(bool start_enabled = true);
	~ModuleImporter();

	bool Init(const nlohmann::json::iterator& it);
	bool Start();
	bool CleanUp();
	bool Load(const nlohmann::json::iterator& it);
	bool Save(nlohmann::json &it);
	void ImportFile(const char* path);

public:
	MeshImporter* mesh;
	TextureImporter* texture;
	ModelImporter* model;
};

#endif