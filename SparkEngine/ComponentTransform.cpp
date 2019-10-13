#include "ComponentTransform.h"


int ComponentTransform::GetChildCount()
{
	return children.size();
}

void ComponentTransform::SetParent(ComponentTransform* parent)
{
	if (parent != nullptr)
	{
		this->parent = parent;
		//transform_matrix = parent->transform_matrix * local_transform_matrix;
	}
	else {
		//transform_matrix = {
		//{ 1.0f, 0.0f, 0.0f, 0.0f },
		//{ 0.0f, 1.0f, 0.0f, 0.0f },
		//{ 0.0f, 0.0f, 1.0f, 0.0f },
		//{ 0.0f, 0.0f, 0.0f, 0.0f }
		//};
	}
}

ComponentTransform * ComponentTransform::GetParent()
{
	return this->parent;
}

float3 ComponentTransform::EulerAngles()
{
	return rotation.ToEulerXYZ();
}

ComponentTransform::ComponentTransform():Component(gameobject)
{
	//local_transform_matrix = {
	//	{ 1.0f, 0.0f, 0.0f, 0.0f },
	//	{ 0.0f, 1.0f, 0.0f, 0.0f },
	//	{ 0.0f, 0.0f, 1.0f, 0.0f },
	//	{ 0.0f, 0.0f, 0.0f, 0.0f }
	//};

	position = { 0.0f, 0.0f, 0.0f };
	rotation = { 0.0f, 0.0f, 0.0f, 0.0f };
	scale = { 0.0f, 0.0f, 0.0f };	
	
	local_position = { 0.0f, 0.0f, 0.0f };
	local_rotation = { 0.0f, 0.0f, 0.0f, 0.0f };
	local_scale = { 0.0f, 0.0f, 0.0f };
}


ComponentTransform::~ComponentTransform()
{
}

void ComponentTransform::Update(float dt)
{
	//transform_matrix = parent->transform_matrix * local_transform_matrix;

	//transform_matrix.Decompose(position, rotation, scale);
}
