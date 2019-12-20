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
		R_MODEL,
		R_SCENE,
		R_MATERIAL,
		R_ANIMATION,
		R_BONE,
		R_NONE
	};

	struct MetaFile {
		bool loaded = false;

		int modification_date;
		std::string file;
		std::string original_file;
		std::string exported_file;
		uint id;

		virtual void SetDefault() {}
	};

public:
	~Resource();
	Resource(uint id, RESOURCE_TYPE type);

	RESOURCE_TYPE GetType() const;
	uint GetID() const;
	const char* GetFile() const;
	const char* GetExportedFile() const;
	bool IsLoaded() const;
	void AddReference();
	void RemoveReference();
	void SetFile(std::string f);
	void SetExportedFile(std::string f);
	virtual void Load() {}
	virtual void UnLoad() {}
	uint GetReferences();
	virtual std::string GetTypeString() const { return ""; }
	virtual void ReLoad() {}

	MetaFile* meta = nullptr;
	uint references = 0;
protected:
	//virtual void Save(Config& config) const;
	//virtual void Load(const Config& config);
	//virtual bool LoadInMemory() = 0;

protected:
	uint id = 0;
	std::string file;
	std::string exported_file;
	RESOURCE_TYPE type = RESOURCE_TYPE::R_NONE;
};

#endif // !_RESOURCE_H_

