#include "ComponentTransform.h"
#include "Globals.h"

#include "GameObject.h"
#include "ComponentCamera.h"


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

void ComponentTransform::RemoveChild(ComponentTransform* child)
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

bool ComponentTransform::IsDirectChild(ComponentTransform * ct)
{
	bool ret = false;

	for (int i = 0; i < children.size() && !ret; i++)
	{
		if (children[i] == ct)
			return true;
	}

	return ret;
}

float3 ComponentTransform::EulerAngles()
{
	return local_rotation.ToEulerXYZ();
}

void ComponentTransform::UpdateTransformMatrix()
{
	if (parent)transform_matrix = parent->GetTransformMatrix() * local_transform_matrix;
	else transform_matrix = local_transform_matrix;

	UpdateChildrenTransformMatrix();
	//Update other components matrix that depend of this transform matrix such as frustrum and bbox
	UpdateOtherComponentsTransform();

	transform_matrix.Decompose(position, rotation, scale);
}

void ComponentTransform::UpdateLocalTransformMatrix()
{
	local_transform_matrix = parent->GetTransformMatrix().Inverted() * transform_matrix;

	local_transform_matrix.Decompose(local_position, local_rotation, local_scale);
}

void ComponentTransform::UpdateChildrenTransformMatrix()
{
	if (children.size() > 0)
	{
		for (int i = 0; i < children.size(); i++) {
			children[i]->UpdateTransformMatrix();
		}
	}
}

void ComponentTransform::UpdateOtherComponentsTransform()
{
	//Update bbox when a transformation happen
	if (gameobject->HasComponent(COMPONENT_TYPE::MESH))gameobject->UpdateBBox();

	//If the gameobject attached has a component camera, we update the frustrum tranform
	if (gameobject->HasComponent(COMPONENT_TYPE::CAMERA)) {
		ComponentCamera* c_cam = (ComponentCamera*)gameobject->GetComponent(COMPONENT_TYPE::CAMERA);
		c_cam->UpdateFrustumTransform();
	}
}

float4x4 ComponentTransform::GetTransformMatrix() const
{
	return transform_matrix;
}

void ComponentTransform::SetTransformMatrix(const float4x4 & transform_matrix)
{
	this->transform_matrix = transform_matrix;
	UpdateLocalTransformMatrix();

	UpdateChildrenTransformMatrix();
	UpdateOtherComponentsTransform();

	transform_matrix.Decompose(position, rotation, scale);
}

float4x4 ComponentTransform::GetLocalTransformMatrix() const
{
	return local_transform_matrix;
}

void ComponentTransform::SetLocalTransformMatrix(const float4x4 & local_transform_matrix)
{
	this->local_transform_matrix = local_transform_matrix;
	UpdateTransformMatrix();
}

float3 ComponentTransform::GetPosition() const
{
	return position;
}

Quat ComponentTransform::GetRotation() const
{
	return rotation;
}

float3 ComponentTransform::GetRotationToEuler() const
{
	return rotation.ToEulerXYZ() * RADTODEG;
}

float3 ComponentTransform::GetScale() const
{
	return scale;
}

void ComponentTransform::SetRotation(const Quat & rotation)
{
	this->rotation = rotation;
	SetTransformMatrix(float4x4::FromTRS(position, this->rotation, scale));
}

void ComponentTransform::SetRotationFromEuler(const float3 & euler_rotation)
{
	rotation = Quat::FromEulerXYZ(euler_rotation.x * DEGTORAD, euler_rotation.y* DEGTORAD, euler_rotation.z* DEGTORAD);
	SetTransformMatrix(float4x4::FromTRS(position, this->rotation, scale));
}

void ComponentTransform::SetPosition(const float3 & position)
{
	this->position = position;
	SetTransformMatrix(float4x4::FromTRS(this->position, rotation, scale));
}

void ComponentTransform::SetScale(const float3 & scale)
{
	this->scale = scale;
	SetTransformMatrix(float4x4::FromTRS(position, rotation, this->scale));
}

float3 ComponentTransform::GetLocalPosition() const
{
	return local_position;
}

Quat ComponentTransform::GetLocalRotation() const
{
	return local_rotation;
}

float3 ComponentTransform::GetLocalRotationToEuler() const
{
	return local_rotation.ToEulerXYZ() * RADTODEG;
}

float3 ComponentTransform::GetLocalScale() const
{
	return local_scale;
}

void ComponentTransform::SetLocalRotationFromEuler(const float3 & euler_rotation)
{
	local_rotation = Quat::FromEulerXYZ(euler_rotation.x * DEGTORAD, euler_rotation.y* DEGTORAD, euler_rotation.z* DEGTORAD);
	SetLocalTransformMatrix(float4x4::FromTRS(local_position, local_rotation, local_scale));
}

void ComponentTransform::SetLocalRotation(const Quat & rotation)
{
	local_rotation = rotation;
	SetLocalTransformMatrix(float4x4::FromTRS(local_position, local_rotation, local_scale));
}

void ComponentTransform::SetLocalPosition(const float3 & position)
{
	local_position = position;
	SetLocalTransformMatrix(float4x4::FromTRS(local_position, local_rotation, local_scale));
}

void ComponentTransform::SetLocalScale(const float3 & scale)
{
	local_scale = scale;
	SetLocalTransformMatrix(float4x4::FromTRS(local_position, local_rotation, local_scale));
}

ComponentTransform::ComponentTransform(GameObject* gameobject):Component(gameobject)
{
	parent = nullptr;

	position = { 0.0f, 0.0f, 0.0f };
	rotation = { 0.0f, 0.0f, 0.0f, 1.0f };
	scale = { 1.0f, 1.0f, 1.0f };	
	
	local_position = { 0.0f, 0.0f, 0.0f };
	local_rotation = { 0.0f, 0.0f, 0.0f, 1.0f };
	local_scale = { 1.0f, 1.0f, 1.0f };

	local_transform_matrix = float4x4::FromTRS(local_position, local_rotation, local_scale);
	transform_matrix = float4x4::FromTRS(position, rotation, scale);
}


ComponentTransform::~ComponentTransform()
{

}

bool ComponentTransform::Save(const nlohmann::json::iterator& it)
{
	nlohmann::json object = {
		{"type", type},
		{"position", { local_position.x, local_position.y, local_position.z }},
		{"rotation", { local_rotation.x, local_rotation.y, local_rotation.z, local_rotation.w} },
		{"scale", { local_scale.x, local_scale.y, local_scale.z} }
	};

	it.value().push_back(object);

	return true;
}

bool ComponentTransform::Load(const nlohmann::json comp)
{
	type = comp["type"];
	float3 load_position = float3(comp["position"][0], comp["position"][1], comp["position"][2]);
	Quat load_rotation = Quat(comp["rotation"][0], comp["rotation"][1], comp["rotation"][2], comp["rotation"][3]);
	float3 load_scale = float3(comp["scale"][0], comp["scale"][1], comp["scale"][2]);

	SetLocalPosition(load_position);
	SetLocalRotation(load_rotation);
	SetLocalScale(load_scale);

	return true;
}
