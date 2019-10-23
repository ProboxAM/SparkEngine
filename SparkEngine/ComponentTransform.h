#ifndef _COMPONENT_TRANSFORM_H_
#define _COMPONENT_TRANSFORM_H_

#include "Component.h"
#include "MathGeoLib/Math/float4x4.h"
#include "MathGeoLib/Math/Quat.h"
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
	void SetParent(ComponentTransform* parent);
	ComponentTransform* GetParent();
	std::vector<ComponentTransform*> GetChildren();
	float3 EulerAngles();

	void SetTransformMatrix(float4x4  matrix);
	float4x4 GetTransformMatrix();


public:
	ComponentTransform(GameObject* gameobject);
	~ComponentTransform();

	void Update(float dt);
};

#endif // !_COMPONENT_TRANSFORM_H_



