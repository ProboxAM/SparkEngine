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

	//global
	float3 position;
	Quat rotation;
	float3 scale;

	//local
	float3 local_position;
	Quat local_rotation;
	float3 local_scale;

public:
	int GetChildCount();
	void SetParent(ComponentTransform* parent);
	ComponentTransform* GetParent();
	std::vector<ComponentTransform*> GetChildren();
	void AddChild(ComponentTransform* child);
	void RemoveChild(ComponentTransform* child);
	bool IsChild(ComponentTransform* ct);
	float3 EulerAngles();

	void UpdateTransformMatrix();
	void UpdateLocalTransformMatrix();
	void UpdateChildrenTransformMatrix();
	void UpdateOtherComponentsTransform();

	float4x4 GetTransformMatrix();
	void SetTransformMatrix(float4x4 transform_matrix);
	float4x4 GetLocalTransformMatrix();
	void SetLocalTransformMatrix(float4x4 local_transform_matrix);

	//global
	float3 GetPosition();
	Quat GetRotation();
	float3 GetRotationToEuler();
	float3 GetScale();
	void SetRotation(Quat rotation);
	void SetRotationFromEuler(float3 euler_rotation);
	void SetPosition(float3 position);
	void SetScale(float3 scale);

	//local
	float3 GetLocalPosition();
	Quat GetLocalRotation();
	float3 GetLocalRotationToEuler();
	float3 GetLocalScale();
	void SetLocalRotationFromEuler(float3 euler_rotation);
	void SetLocalRotation(Quat rotation);
	void SetLocalPosition(float3 position);
	void SetLocalScale(float3 scale);

	bool Save(const nlohmann::json::iterator& it);

public:
	ComponentTransform(GameObject* gameobject);
	~ComponentTransform();

	void Update(float dt);
};

#endif // !_COMPONENT_TRANSFORM_H_



