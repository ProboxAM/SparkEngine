#include "ComponentTransform.h"


int ComponentTransform::GetChildCount()
{
	return children.size();
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
	this->gameobject = gameobject;
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
