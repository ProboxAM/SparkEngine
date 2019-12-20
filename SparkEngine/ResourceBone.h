#ifndef _RESOURCE_BONE_H_
#define _RESOURCE_BONE_H_

#include "Resource.h"
#include "MathGeoLib\Math\float4x4.h"

class ResourceBone :
	public Resource
{
public:
	ResourceBone(uint id);
	~ResourceBone();

	std::string name;
	float4x4 matrix;

	uint num_weights;
	uint* vertex_ids;
	float* weights;

	std::string GetTypeString() const;
	void Load();
	void UnLoad();
};

#endif // !_RESOURCE_BONE_H_


