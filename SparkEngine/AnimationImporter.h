#ifndef _ANIMATION_IMPORTER_H_
#define _ANIMATION_IMPORTER_H

#include "Importer.h"

class ResourceAnimation;
struct aiScene;
struct aiAnimation;

class AnimationImporter :
	public Importer
{
public:
	AnimationImporter();
	~AnimationImporter();

	uint Import(const char* file, const aiAnimation* anim, uint id);
	bool SaveAnimation(ResourceAnimation* anim);

	bool Load(ResourceAnimation* resource);
};

#endif // !_ANIMATION_IMPORTER_H_



