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
	uint ImportFile(const char* new_file_in_assets, Resource::RESOURCE_TYPE type, MetaFile* meta = nullptr);
	uint GenerateNewUID();
	Resource * GetAndReference(uint id);
	Resource* Get(uint uid);
	uint GetID(std::string file);
	Resource* CreateResource(Resource::RESOURCE_TYPE type, uint id);
	Resource* CreateResource(Resource::RESOURCE_TYPE type, uint id, bool& needs_reload);
	uint GetIDFromMeta(std::string file);
	std::map<uint, Resource*> GetResources();
	std::vector<Resource*> GetResources(Resource::RESOURCE_TYPE type);
	Resource::RESOURCE_TYPE GetTypeFromExtension(std::string extension);

private:
	void RecursiveLoadAssets(std::string directory);
	bool ImportedLibraryFilesExist(MetaFile * meta, Resource::RESOURCE_TYPE type);

	bool LoadMeta(const char * file, MetaFile* meta, Resource::RESOURCE_TYPE type);
	MetaFile* CreateMeta(const char * file, Resource::RESOURCE_TYPE type);
	void CreateResourcesFromMeta(MetaFile * meta, Resource::RESOURCE_TYPE type);

	void CleanLibrary();
	void CleanLibraryFolder(const char * folder);

private:
	uint last_id = 1;
	std::map<uint, Resource*> resources;
};

#endif // !_MODULE_RESOURCES_H_



