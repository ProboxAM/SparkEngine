#ifndef _MODULE_RESOURCES_H_
#define _MODULE_RESOURCES_H_

#include "Module.h"
#include "Resource.h"

class ModuleResources : public Module
{
public:
	ModuleResources(bool active);
	~ModuleResources();

	bool Start();

	bool ImportFileToAssets(const char* path);
	uint ImportFile(const char* new_file_in_assets, Resource::RESOURCE_TYPE type);
	uint GenerateNewUID();
	Resource * GetAndLoad(uint id);
	Resource* Get(uint uid);
	uint GetID(std::string file);
	Resource* CreateResource(Resource::RESOURCE_TYPE type);
	Resource* CreateResource(Resource::RESOURCE_TYPE type, uint id);

private:
	Resource::RESOURCE_TYPE GetTypeFromExtension(std::string extension);
	bool LoadResource(Resource* resource);
	void LoadAssets();
	uint GetIDFromMeta(std::string file);

	bool LoadMeta(const char * file, Resource::MetaFile* meta, Resource::RESOURCE_TYPE type);

	Resource::MetaFile* CreateMeta(const char * file, Resource::RESOURCE_TYPE type);

private:
	uint last_id = 1;
	std::map<uint, Resource*> resources;
};

#endif // !_MODULE_RESOURCES_H_



