#include "GameObject.h"
#include "ComponentTransform.h"



void GameObject::Update(float dt)
{

}

Component* GameObject::AddComponent(COMPONENT_TYPE type)
{
	Component* c;

	switch (type)
	{
	case COMPONENT_TYPE::TRANSFORM:
		c = new ComponentTransform();
		break;
	case COMPONENT_TYPE::MESH:
		break;
	case COMPONENT_TYPE::MATERIAL:
		break;
	case COMPONENT_TYPE::LIGHT:
		break;
	default:
		break;
	}

	components.push_back(c);
	return c;
}

Component * GameObject::GetComponent(COMPONENT_TYPE type)
{
	Component* aux;
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
		if (components[i]->type == type)
		{
			aux.push_back(components[i]);
		}
	}

	return aux;
}

bool GameObject::isActive()
{
	return active;
}

bool GameObject::isStatic()
{
	return is_static;
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

GameObject::GameObject()
{
	this->transform = (ComponentTransform*)AddComponent(COMPONENT_TYPE::TRANSFORM);
}


GameObject::~GameObject()
{
}
