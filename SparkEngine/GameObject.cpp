#include "Application.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "ComponentTexture.h"
#include "ComponentCamera.h"
#include "GameObject.h"


void GameObject::Update()
{
	if (active)
	{
		for (int i = 0; i < components.size(); i++)
		{
			if (components[i]->active)
				components[i]->Update();
		}

		for (int i = 0; i < transform->GetChildCount(); i++)
		{
			transform->GetChildren()[i]->gameobject->Update();
		}
	}
}

void GameObject::Delete()
{
	std::vector<ComponentTransform*> children = transform->GetChildren();
	for (std::vector<ComponentTransform*>::iterator it = children.begin(); it != children.end(); ++it)
	{
		(*it)->gameobject->Delete();
	}

	for (std::vector<Component*>::iterator it = components.begin(); it != components.end(); ++it)
	{
		delete (*it);
	}
	components.clear();

	transform = nullptr;
	delete this;
}

void GameObject::Draw()
{
	if (active)
	{
		for each(Component* comp in components)
		{
			comp->Draw();
		}

		for (int i = 0; i < transform->GetChildCount(); i++)
		{
			transform->GetChildren()[i]->gameobject->Draw();
		}
	}
		
}

Component* GameObject::AddComponent(COMPONENT_TYPE type)
{
	Component* c;

	switch (type)
	{
	case COMPONENT_TYPE::TRANSFORM:
		c = new ComponentTransform(this);
		break;
	case COMPONENT_TYPE::MESH:
		c = new ComponentMesh(this);
		break;
	case COMPONENT_TYPE::TEXTURE:
		c = new ComponentTexture(this);
		break;
	case COMPONENT_TYPE::LIGHT:
		break;	
	case COMPONENT_TYPE::CAMERA:
		c = new ComponentCamera(this);
		break;
	default:
		break;
	}
	c->type = type;

	components.push_back(c);
	return c;
}

Component * GameObject::GetComponent(COMPONENT_TYPE type) const
{
	Component* aux = nullptr;
	for (int i = 0; i < components.size(); i++)
	{
		if (components[i]->type == type)
		{
			aux = components[i];
			break;
		}
	}

	return aux;
}

std::vector<Component*> GameObject::GetComponents(COMPONENT_TYPE type)
{
	std::vector<Component*> aux;

	for (int i = 0; i < components.size(); i++)
	{
		if (type != NONE)
		{
			if (components[i]->type == type)
			{
				aux.push_back(components[i]);
			}
		}else aux.push_back(components[i]);
	}

	return aux;
}

std::vector<Component*> GameObject::GetComponents()
{
	return components;
}

bool GameObject::HasComponent(COMPONENT_TYPE type)
{
	bool ret = false;
	for (int i = 0; i < components.size(); i++)
	{
		if (components[i]->type == type) ret = true;
	}

	return ret;
}

void GameObject::SetName(std::string name)
{
	this->name = name;
}

std::string GameObject::GetName() const
{
	return name;
}

void GameObject::SetTag(std::string tag)
{
	this->tag = tag;
}

std::string GameObject::GetTag() const
{
	return tag;
}

bool GameObject::isActive() const
{
	return active;
}

bool GameObject::isStatic() const
{
	return is_static;
}

void GameObject::SetStatic(bool state)
{
	is_static = state;
}

void GameObject::SetActive(bool active)
{
	this->active = active;
}

bool GameObject::CompareTag(std::string tag)
{
	if (this->tag == tag) return true;
	else return false;
}

bool GameObject::Save(const nlohmann::json::iterator& it)
{
	nlohmann::json object = {
		{"id", id},
		{"parent", transform->GetParent()? transform->GetParent()->gameobject->GetId() : 0},
		{"name", name},
		{"components", nlohmann::json::array()},
		{"layer", layer},
		{"active", active},
		{"static", is_static},
		{"tag", tag}
	};

	for each (Component * comp in components) //Save components
	{
		comp->Save(object.find("components"));
	}

	it.value().push_back(object);

	//Save children
	std::vector<ComponentTransform*>childs = transform->GetChildren();
	for each (ComponentTransform* child in childs)
	{
		child->gameobject->Save(it);
	}

	return true;
}

float GameObject::GetDistanceFromAABB()
{
	float distance = 0;

	if (HasComponent(COMPONENT_TYPE::MESH)) {
		distance = global_aabb.Size().Length();
	}
	else LOG("Object has no mesh, cant return a valid AABB distance");

	return distance;
}

uint GameObject::GetId()
{
	return id;
}

void GameObject::UpdateBBox()
{
	ComponentMesh* mesh = (ComponentMesh*)GetComponent(COMPONENT_TYPE::MESH);
	global_obb = mesh->GetAABB();
	global_obb.Transform(transform->GetTransformMatrix());

	global_aabb.SetNegativeInfinity();
	global_aabb.Enclose(global_obb);
}

GameObject::GameObject()
{
	this->transform = (ComponentTransform*)AddComponent(COMPONENT_TYPE::TRANSFORM);
	id = App->GenerateID();
}

GameObject::GameObject(uint id)
{
	this->transform = (ComponentTransform*)AddComponent(COMPONENT_TYPE::TRANSFORM);
	this->id = id;
}


GameObject::~GameObject()
{
}
