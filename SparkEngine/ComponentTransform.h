#pragma once
#include "Component.h"
#include "MathGeoLib/Math/MathAll.h"

class ComponentTransform :
	public Component
{
private:
	float4x4 transform_matrix;
	float4x4 local_transform_matrix;
public:

	//global
	float3 position;
	float3 rotation;
	float3 scale;

	//local
	float3 local_position;
	float3 local_rotation;
	float3 local_scale;

public:
	ComponentTransform();
	~ComponentTransform();
};

