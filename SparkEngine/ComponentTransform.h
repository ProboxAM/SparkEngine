#ifndef _COMPONENT_TRANSFORM_H_
#define _COMPONENT_TRANSFORM_H_

#include "Component.h"
#include "MathGeoLib/Math/MathAll.h"
#include <vector>
#include <algorithm>

class ComponentTransform :
	public Component
{
private:
	ComponentTransform* parent = nullptr;
	std::vector<ComponentTransform*> children;
	float4x4 local_transform_matrix;
	float4x4 transform_matrix;

public:
	//global
	float3 position;
	float3 euler_rotation;
	Quat rotation;
	float3 scale;

	//local
	float3 local_position;
	float3 local_euler_rotation;
	Quat local_rotation;
	float3 local_scale;

	int GetChildCount();
	void SetParent(ComponentTransform* parent);
	ComponentTransform* GetParent();
	std::vector<ComponentTransform*> GetChildren();
	void AddChild(ComponentTransform* child);
	void DestroyChild(ComponentTransform* child);
	bool IsChild(ComponentTransform* ct);
	float3 EulerAngles();

	void UpdateTransformMatrix();
	float4x4 GetTransformMatrix();

	bool Save(const nlohmann::json::iterator& it);

public:
	ComponentTransform(GameObject* gameobject);
	~ComponentTransform();

	void Update(float dt);
};

#endif // !_COMPONENT_TRANSFORM_H_



