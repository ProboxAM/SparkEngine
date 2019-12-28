#ifndef _RESOURCE_MODEL_H_
#define _RESOURCE_MODEL_H_

#include "Resource.h"
#include "MathGeoLib\Math\Quat.h"
#include <map>

class ResourceModel : public Resource
{
public:
	struct ModelNode
	{
		std::string name;
		float3 position;
		float3 scale;
		Quat rotation;

		uint parent;
		uint mesh = 0;
		uint texture = 0;
		uint bone = 0;

		bool root_bone = false;
		uint mesh_binded = 0;
	};

public:
	ResourceModel(uint id);
	~ResourceModel();

	void UnLoad();
	void ReLoad();
	void Load();

	std::vector<ModelNode> nodes;
	std::string GetTypeString() const;
};


#endif // !_RESOURCE_MODEL_H_

