#ifndef _BONE_IMPORTER_H_
#define _BONE_IMPORTER_H_

#include "Importer.h"
class ResourceBone;
class aiBone;

class BoneImporter :
	public Importer
{
public:
	BoneImporter();
	~BoneImporter();

	uint Import(const char* file, const aiBone* bone, uint id);
	bool SaveBone(ResourceBone* bone);

	bool Load(ResourceBone* resource);
};

#endif
