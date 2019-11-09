#ifndef _MODULE_RESOURCES_H_
#define _MODULE_RESOURCES_H_

#include "Module.h"
#include "Resource.h"

class ModuleResources : public Module
{
public:
	ModuleResources();
	~ModuleResources();

	uint Find(const char* file_in_assets) const;
	bool ImportFileToAssets(const char* path);
	bool ImportFile(const char* new_file_in_assets, Resource::RESOURCE_TYPE type);
	uint GenerateNewUID();
	Resource* Get(uint uid);
	Resource* CreateNewResource(Resource::RESOURCE_TYPE type, uint force_uid = 0);

private:
	Resource::RESOURCE_TYPE GetTypeFromExtension(const char* extension);

private:
	uint last_id = 1;
	std::map<uint, Resource*> resources;
};

#endif // !_MODULE_RESOURCES_H_



