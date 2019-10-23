#include "ComponentTransform.h"
#include "Globals.h"



int ComponentTransform::GetChildCount()
{
	return children.size();
}

void ComponentTransform::SetParent(ComponentTransform* parent)
{
	this->parent = parent;
	this->parent->children.push_back(this);
}


ComponentTransform * ComponentTransform::GetParent()
{
	return this->parent;
}

std::vector<ComponentTransform*> ComponentTransform::GetChildren()
{
	return children;
}

float3 ComponentTransform::EulerAngles()
{
	return local_rotation.ToEulerXYZ();
}

float4x4 ComponentTransform::GetTransformMatrix()
{
	float4x4 local_transform_matrix(local_rotation, local_position);
	local_transform_matrix.Scale(local_scale);

	float4x4 transform_matrix;

	if (parent)transform_matrix = parent->GetTransformMatrix() * local_transform_matrix;
	else transform_matrix = local_transform_matrix;

	return transform_matrix;
}

ComponentTransform::ComponentTransform(GameObject* gameobject):Component(gameobject)
{
	position = { 0.0f, 0.0f, 0.0f };
	rotation = { 0.0f, 0.0f, 0.0f, 0.0f };
	scale = { 1.0f, 1.0f, 1.0f };	
	
	local_position = { 0.0f, 0.0f, 0.0f };
	local_rotation = { 0.0f, 0.0f, 0.0f, 0.0f };
	local_scale = { 1.0f, 1.0f, 1.0f };
}


ComponentTransform::~ComponentTransform()
{
}

void ComponentTransform::Update(float dt)
{
	
}
