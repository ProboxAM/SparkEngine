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
	bool IsDirectChild(ComponentTransform* ct);
	float3 EulerAngles();

	void UpdateTransformMatrix();
	void UpdateLocalTransformMatrix();
	void UpdateChildrenTransformMatrix();
	void UpdateOtherComponentsTransform();

	float4x4 GetTransformMatrix() const;
	void SetTransformMatrix(const float4x4 &transform_matrix);
	float4x4 GetLocalTransformMatrix() const;
	void SetLocalTransformMatrix(const float4x4 &local_transform_matrix);

	//global
	float3 GetPosition() const;
	Quat GetRotation() const;
	float3 GetRotationToEuler() const;
	float3 GetScale() const;
	void SetRotation(const Quat &rotation);
	void SetRotationFromEuler(const float3 &euler_rotation);
	void SetPosition(const float3 &position);
	void SetScale(const float3 &scale);

	//local
	float3 GetLocalPosition() const;
	Quat GetLocalRotation() const;
	float3 GetLocalRotationToEuler() const;
	float3 GetLocalScale() const;
	void SetLocalRotationFromEuler(const float3 &euler_rotation);
	void SetLocalRotation(const Quat &rotation);
	void SetLocalPosition(const float3 &position);
	void SetLocalScale(const float3 &scale);

	bool Save(const nlohmann::json::iterator& it);
	bool Load(const nlohmann::json comp);

public:
	ComponentTransform(GameObject* gameobject);
	virtual ~ComponentTransform();
};

#endif // !_COMPONENT_TRANSFORM_H_



