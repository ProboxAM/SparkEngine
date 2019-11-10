#ifndef _RESOURCE_MODEL_H_
#define _RESOURCE_MODEL_H_

#include "Resource.h"
#include "MathGeoLib\Math\Quat.h"

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
	};
public:
	ResourceModel(uint id);
	~ResourceModel();

	std::vector<ModelNode> nodes;
};


#endif // !_RESOURCE_MODEL_H_

