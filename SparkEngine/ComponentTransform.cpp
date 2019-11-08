#include "ComponentTransform.h"
#include "Globals.h"

#include "GameObject.h"


int ComponentTransform::GetChildCount()
{
	return children.size();
}

void ComponentTransform::SetParent(ComponentTransform* parent)
{
	this->parent = parent;
}


ComponentTransform * ComponentTransform::GetParent()
{
	return this->parent;
}

std::vector<ComponentTransform*> ComponentTransform::GetChildren()
{
	return children;
}

void ComponentTransform::AddChild(ComponentTransform* child)
{
	children.push_back(child);
}

void ComponentTransform::DestroyChild(ComponentTransform* child)
{
	children.erase(remove(children.begin(), children.end(), child));
}

bool ComponentTransform::IsChild(ComponentTransform * ct)
{
	bool ret = false;

	for (int i = 0; i < children.size() && !ret; i++)
	{
		if (children[i] == ct)
			return true;

		ret = children[i]->IsChild(ct);
	}

	return ret;
}

float3 ComponentTransform::EulerAngles()
{
	return local_rotation.ToEulerXYZ();
}

void ComponentTransform::UpdateTransformMatrix()
{
	local_rotation = Quat::FromEulerXYZ(local_euler_rotation.x * DEGTORAD, local_euler_rotation.y* DEGTORAD, local_euler_rotation.z* DEGTORAD);
	local_transform_matrix = float4x4::FromTRS(local_position, local_rotation, local_scale);

	if (parent)transform_matrix = parent->GetTransformMatrix() * local_transform_matrix;
	else transform_matrix = local_transform_matrix;

	if (children.size() > 0)
	{
		for (int i = 0; i < children.size(); i++) {
			children[i]->UpdateTransformMatrix();
		}
	}

	//Update bbox when a transformation happen
	if(gameobject->HasComponent(COMPONENT_TYPE::MESH))gameobject->UpdateBBox();

	transform_matrix.Decompose(position, rotation, scale);
}

float4x4 ComponentTransform::GetTransformMatrix() {
	return transform_matrix;
}

ComponentTransform::ComponentTransform(GameObject* gameobject):Component(gameobject)
{
	parent = nullptr;

	position = { 0.0f, 0.0f, 0.0f };
	rotation = { 0.0f, 0.0f, 0.0f, 1.0f };
	euler_rotation = { 0.0f, 0.0f, 0.0f };
	scale = { 1.0f, 1.0f, 1.0f };	
	
	local_position = { 0.0f, 0.0f, 0.0f };
	local_rotation = { 0.0f, 0.0f, 0.0f, 1.0f };
	local_euler_rotation = { 0.0f, 0.0f, 0.0f };
	local_scale = { 1.0f, 1.0f, 1.0f };

	local_transform_matrix = float4x4::FromTRS(local_position, local_rotation, local_scale);
	transform_matrix = float4x4::FromTRS(position, rotation, scale);
}


ComponentTransform::~ComponentTransform()
{
}

void ComponentTransform::Update(float dt)
{

}


/*bool ComponentTransform::Save()
{
	{"translation", { transform->position.x, transform->position.y, transform->position.z }},
	{ "rotation",{transform->rotation.x, transform->rotation.y, transform->rotation.z, transform->rotation.w} },
	{ "scale",{transform->scale.x, transform->scale.y, transform->scale.z} },
}*/
