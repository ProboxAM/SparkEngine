#ifndef _ANIMATION_IMPORTER_H_
#define _ANIMATION_IMPORTER_H

#include "Importer.h"
#include "ResourceAnimation.h"

struct aiScene;
struct aiAnimation;
class AnimationMetaFile;

class AnimationImporter :
	public Importer
{
public:
	AnimationImporter();
	~AnimationImporter();

	AnimationMetaFile* Import(const char* file, const aiAnimation* anim, AnimationMetaFile* meta = nullptr);
	bool SaveAnimation(ResourceAnimation* anim);

	bool Load(ResourceAnimation* resource);
};

#endif // !_ANIMATION_IMPORTER_H_



