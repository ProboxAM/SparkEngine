#ifndef _RESOURCE_H_
#define _RESOURCE_H_

#include <vector>
#include "Globals.h"
#include <string>

class Resource
{
public:
	enum RESOURCE_TYPE {
		R_TEXTURE,
		R_MESH,
		R_SCENE,
		R_NONE
	};
public:
	~Resource();
	Resource(uint id, RESOURCE_TYPE type);

	RESOURCE_TYPE GetType() const;
	uint GetID() const;
	const char* GetFile() const;
	const char* GetExportedFile() const;
	bool IsLoaded() const;
	bool Load();
	uint CountReferences() const;

protected:
	//virtual void Save(Config& config) const;
	//virtual void Load(const Config& config);
	//virtual bool LoadInMemory() = 0;

protected:
	uint id = 0;
	std::string file;
	std::string exported_file;
	RESOURCE_TYPE type = RESOURCE_TYPE::R_NONE;
	bool loaded = false;
};

#endif // !_RESOURCE_H_

