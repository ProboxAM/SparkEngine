#ifndef _COMPONENT_BONE_H_
#define _COMPONENT_BONE_H_

#include "Component.h"

class ResourceBone;

class ComponentBone :
	public Component
{
public:
	ComponentBone(GameObject* gameobject);
	~ComponentBone();
public:
	bool debug_draw = false;
	void Draw();
	void AddBone(ResourceBone* b);
	uint GetBoneID();
private:
	ResourceBone* bone = nullptr;
};

#endif // !_COMPONENT_BONE_H_



