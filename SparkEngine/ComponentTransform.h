#pragma once
#include "Component.h"
#include "MathGeoLib/Math/MathAll.h"
#include <vector>

class ComponentTransform :
	public Component
{
private:
	float4x4 transform_matrix;
	float4x4 local_transform_matrix;
	ComponentTransform* parent = nullptr;
	std::vector<ComponentTransform*> children;
public:

	//global
	float3 position;
	Quat rotation;
	float3 scale;

	//local
	float3 local_position;
	Quat local_rotation;
	float3 local_scale;

	int GetChildCount();
	ComponentTransform* GetParent();
	
	float3 EulerAngles();
public:
	ComponentTransform();
	~ComponentTransform();
};

